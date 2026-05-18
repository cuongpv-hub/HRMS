#pragma once
#include <memory>
#include <iostream>
#include <string>
#include <utility>
#include "crow.h"
#include "services/employee_service.hpp"

namespace hrms {

class EmployeeController {
private:
    std::shared_ptr<EmployeeService> service_;

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
        // GOTCHA BẢO MẬT: Tuyệt đối không bao giờ được ghi log số CCCD (id_card_number) hay email nhạy cảm
        std::cout << "[SECURE LOG] Action: " << action << " | " << details << std::endl;
    }

public:
    // GOTCHA: Constructor áp dụng Dependency Injection sử dụng std::move và explicit
    explicit EmployeeController(std::shared_ptr<EmployeeService> service)
        : service_(std::move(service)) {}

    void setup_routes(crow::SimpleApp& app) {

        // 1. Endpoint: Lấy danh sách nhân viên hoạt động (GET /api/employees)
        CROW_ROUTE(app, "/api/employees").methods(crow::HTTPMethod::GET)
        ([this](const crow::request& req) {
            if (!verify_rbac_permission(req, "READ_EMPLOYEE")) {
                return crow::response(403, "Forbidden");
            }

            try {
                auto employees = service_->get_active_employees();
                crow::json::wvalue response_json;
                int index = 0;
                for (const auto& e : employees) {
                    response_json[index]["id"] = e.id;
                    response_json[index]["code"] = e.code;
                    response_json[index]["full_name"] = e.full_name;
                    response_json[index]["email"] = e.email;
                    if (e.phone.has_value()) {
                        response_json[index]["phone"] = *e.phone;
                    } else {
                        response_json[index]["phone"] = nullptr;
                    }
                    response_json[index]["status"] = e.status;
                    if (e.department_id.has_value()) {
                        response_json[index]["department_id"] = *e.department_id;
                    } else {
                        response_json[index]["department_id"] = nullptr;
                    }
                    // GOTCHA: Trả về CCCD trên API bảo mật của nhân sự nhưng KHÔNG được in ra console log
                    response_json[index]["id_card_number"] = e.id_card_number;
                    response_json[index]["created_at"] = e.created_at;
                    index++;
                }
                return crow::response(200, response_json);
            } catch (const std::exception& e) {
                return crow::response(500, "Internal Server Error");
            }
        });

        // 2. Endpoint: Lấy chi tiết nhân viên theo ID (GET /api/employees/<int>)
        CROW_ROUTE(app, "/api/employees/<int>").methods(crow::HTTPMethod::GET)
        ([this](const crow::request& req, int id) {
            if (!verify_rbac_permission(req, "READ_EMPLOYEE")) {
                return crow::response(403, "Forbidden");
            }

            try {
                auto e = service_->get_employee_by_id(id);
                if (!e.has_value()) {
                    return crow::response(404, "Employee not found or deleted.");
                }

                crow::json::wvalue e_json;
                e_json["id"] = e->id;
                e_json["code"] = e->code;
                e_json["full_name"] = e->full_name;
                e_json["email"] = e->email;
                if (e->phone.has_value()) {
                    e_json["phone"] = *e->phone;
                }
                e_json["status"] = e->status;
                if (e->department_id.has_value()) {
                    e_json["department_id"] = *e->department_id;
                }
                e_json["id_card_number"] = e->id_card_number; // CCCD
                e_json["created_at"] = e->created_at;

                return crow::response(200, e_json);
            } catch (const std::exception& e) {
                return crow::response(500, "Internal Server Error");
            }
        });

        // 3. Endpoint: Tạo mới nhân viên (POST /api/employees)
        CROW_ROUTE(app, "/api/employees").methods(crow::HTTPMethod::POST)
        ([this](const crow::request& req) {
            if (!verify_rbac_permission(req, "WRITE_EMPLOYEE")) {
                return crow::response(403, "Forbidden");
            }

            try {
                auto body_json = crow::json::load(req.body);
                if (!body_json) {
                    return crow::response(400, "Invalid JSON payload.");
                }

                EmployeeCreateDTO dto;
                dto.code = body_json["code"].s();
                dto.full_name = body_json["full_name"].s();
                dto.email = body_json["email"].s();
                if (body_json.has("phone") && body_json["phone"].t() != crow::json::type::Null) {
                    dto.phone = body_json["phone"].s();
                }
                dto.id_card_number = body_json["id_card_number"].s();
                if (body_json.has("status")) {
                    dto.status = body_json["status"].s();
                } else {
                    dto.status = "probation";
                }
                if (body_json.has("department_id") && body_json["department_id"].t() != crow::json::type::Null) {
                    dto.department_id = body_json["department_id"].i();
                }

                auto created = service_->create_employee(dto);
                
                // GOTCHA BẢO MẬT: Chỉ log Employee Code và ID, TUYỆT ĐỐI KHÔNG được in dto.id_card_number ra log!
                log_secure_action("CREATE_EMPLOYEE", "Employee successfully created with Code: " + created.code + " | ID: " + std::to_string(created.id));

                crow::json::wvalue response_json;
                response_json["id"] = created.id;
                response_json["code"] = created.code;
                response_json["full_name"] = created.full_name;

                return crow::response(201, response_json);
            } catch (const std::invalid_argument& e) {
                return crow::response(400, e.what());
            } catch (const std::exception& e) {
                return crow::response(500, "Internal Server Error");
            }
        });

        // 4. Endpoint: Xóa mềm nhân viên (DELETE /api/employees/<int>)
        CROW_ROUTE(app, "/api/employees/<int>").methods(crow::HTTPMethod::DELETE)
        ([this](const crow::request& req, int id) {
            if (!verify_rbac_permission(req, "WRITE_EMPLOYEE")) {
                return crow::response(403, "Forbidden");
            }

            try {
                bool success = service_->delete_employee(id);
                if (!success) {
                    return crow::response(404, "Employee not found or already deleted.");
                }

                log_secure_action("DELETE_EMPLOYEE", "Employee ID " + std::to_string(id) + " soft-deleted successfully.");
                return crow::response(200, "Employee deleted successfully.");
            } catch (const std::exception& e) {
                return crow::response(500, "Internal Server Error");
            }
        });
    }
};

} // namespace hrms
