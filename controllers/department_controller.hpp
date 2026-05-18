#pragma once
#include <memory>
#include <iostream>
#include <string>
#include <utility>
#include "crow.h"
#include "services/department_service.hpp"

namespace hrms {

class DepartmentController {
private:
    std::shared_ptr<DepartmentService> service_;

    // Helper giả lập kiểm tra quyền truy cập (RBAC)
    bool verify_rbac_permission(const crow::request& req, const std::string& required_permission) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return false;
        }
        return auth_header.find(required_permission) != std::string::npos;
    }

    // Helper ghi log bảo mật
    void log_secure_action(const std::string& action, const std::string& details) {
        std::cout << "[SECURE LOG] Action: " << action << " | " << details << std::endl;
    }

public:
    // GOTCHA: Constructor áp dụng Dependency Injection sử dụng std::move và explicit
    explicit DepartmentController(std::shared_ptr<DepartmentService> service)
        : service_(std::move(service)) {}

    void setup_routes(crow::SimpleApp& app) {

        // 1. Endpoint: Lấy danh sách phòng ban hoạt động (GET /api/departments)
        CROW_ROUTE(app, "/api/departments").methods(crow::HTTPMethod::GET)
        ([this](const crow::request& req) {
            if (!verify_rbac_permission(req, "READ_DEPARTMENT")) {
                return crow::response(403, "Forbidden");
            }

            try {
                auto departments = service_->get_active_departments();
                crow::json::wvalue response_json;
                int index = 0;
                for (const auto& d : departments) {
                    response_json[index]["id"] = d.id;
                    response_json[index]["code"] = d.code;
                    response_json[index]["name"] = d.name;
                    if (d.manager_id.has_value()) {
                        response_json[index]["manager_id"] = *d.manager_id;
                    } else {
                        response_json[index]["manager_id"] = nullptr;
                    }
                    response_json[index]["created_at"] = d.created_at;
                    index++;
                }
                return crow::response(200, response_json);
            } catch (const std::exception& e) {
                return crow::response(500, "Internal Server Error");
            }
        });

        // 2. Endpoint: Lấy chi tiết phòng ban theo ID (GET /api/departments/<int>)
        CROW_ROUTE(app, "/api/departments/<int>").methods(crow::HTTPMethod::GET)
        ([this](const crow::request& req, int id) {
            if (!verify_rbac_permission(req, "READ_DEPARTMENT")) {
                return crow::response(403, "Forbidden");
            }

            try {
                auto d = service_->get_department_by_id(id);
                if (!d.has_value()) {
                    return crow::response(404, "Department not found or deleted.");
                }

                crow::json::wvalue d_json;
                d_json["id"] = d->id;
                d_json["code"] = d->code;
                d_json["name"] = d->name;
                if (d->manager_id.has_value()) {
                    d_json["manager_id"] = *d->manager_id;
                }
                d_json["created_at"] = d->created_at;

                return crow::response(200, d_json);
            } catch (const std::exception& e) {
                return crow::response(500, "Internal Server Error");
            }
        });

        // 3. Endpoint: Tạo mới phòng ban (POST /api/departments)
        CROW_ROUTE(app, "/api/departments").methods(crow::HTTPMethod::POST)
        ([this](const crow::request& req) {
            if (!verify_rbac_permission(req, "WRITE_DEPARTMENT")) {
                return crow::response(403, "Forbidden");
            }

            try {
                auto body_json = crow::json::load(req.body);
                if (!body_json) {
                    return crow::response(400, "Invalid JSON payload.");
                }

                DepartmentCreateDTO dto;
                dto.code = body_json["code"].s();
                dto.name = body_json["name"].s();
                if (body_json.has("manager_id") && body_json["manager_id"].t() != crow::json::type::Null) {
                    dto.manager_id = body_json["manager_id"].i();
                }

                auto created = service_->create_department(dto);
                log_secure_action("CREATE_DEPARTMENT", "Department successfully created with Code: " + created.code);

                crow::json::wvalue response_json;
                response_json["id"] = created.id;
                response_json["code"] = created.code;
                response_json["name"] = created.name;

                return crow::response(201, response_json);
            } catch (const std::invalid_argument& e) {
                return crow::response(400, e.what());
            } catch (const std::exception& e) {
                return crow::response(500, "Internal Server Error");
            }
        });

        // 4. Endpoint: Xóa mềm phòng ban (DELETE /api/departments/<int>)
        CROW_ROUTE(app, "/api/departments/<int>").methods(crow::HTTPMethod::DELETE)
        ([this](const crow::request& req, int id) {
            if (!verify_rbac_permission(req, "WRITE_DEPARTMENT")) {
                return crow::response(403, "Forbidden");
            }

            try {
                bool success = service_->delete_department(id);
                if (!success) {
                    return crow::response(404, "Department not found or already deleted.");
                }

                log_secure_action("DELETE_DEPARTMENT", "Department ID " + std::to_string(id) + " soft-deleted successfully.");
                return crow::response(200, "Department deleted successfully.");
            } catch (const std::exception& e) {
                return crow::response(500, "Internal Server Error");
            }
        });
    }
};

} // namespace hrms
