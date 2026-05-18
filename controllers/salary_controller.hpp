#pragma once
#include <memory>
#include <iostream>
#include <string>
#include <utility>
#include "crow.h"
#include "services/salary_service.hpp"

namespace hrms {

class SalaryController {
private:
    std::shared_ptr<SalaryService> service_;

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
        // GOTCHA BẢO MẬT: Tuyệt đối không bao giờ được in log số tiền lương cơ bản hay phụ cấp thô
        std::cout << "[SECURE LOG] Action: " << action << " | " << details << std::endl;
    }

public:
    // GOTCHA: Constructor áp dụng Dependency Injection sử dụng std::move và explicit
    explicit SalaryController(std::shared_ptr<SalaryService> service)
        : service_(std::move(service)) {}

    void setup_routes(crow::SimpleApp& app) {

        // 1. Endpoint: Lấy danh sách toàn bộ mức lương hoạt động (GET /api/salaries)
        CROW_ROUTE(app, "/api/salaries").methods(crow::HTTPMethod::GET)
        ([this](const crow::request& req) {
            if (!verify_rbac_permission(req, "READ_SALARY")) {
                return crow::response(403, "Forbidden");
            }

            try {
                auto salaries = service_->get_active_salaries();
                crow::json::wvalue response_json;
                int index = 0;
                for (const auto& s : salaries) {
                    response_json[index]["id"] = s.id;
                    response_json[index]["employee_id"] = s.employee_id;
                    // Trả về số nguyên int64_t ở API bảo mật để tránh sai số thập phân
                    response_json[index]["basic_salary"] = s.basic_salary;
                    response_json[index]["allowance"] = s.allowance;
                    response_json[index]["effective_date"] = s.effective_date;
                    response_json[index]["created_at"] = s.created_at;
                    index++;
                }
                return crow::response(200, response_json);
            } catch (const std::exception& e) {
                return crow::response(500, "Internal Server Error");
            }
        });

        // 2. Endpoint: Lấy chi tiết mức lương theo ID (GET /api/salaries/<int>)
        CROW_ROUTE(app, "/api/salaries/<int>").methods(crow::HTTPMethod::GET)
        ([this](const crow::request& req, int id) {
            if (!verify_rbac_permission(req, "READ_SALARY")) {
                return crow::response(403, "Forbidden");
            }

            try {
                auto s = service_->get_salary_by_id(id);
                if (!s.has_value()) {
                    return crow::response(404, "Salary record not found or deleted.");
                }

                crow::json::wvalue s_json;
                s_json["id"] = s->id;
                s_json["employee_id"] = s->employee_id;
                s_json["basic_salary"] = s->basic_salary;
                s_json["allowance"] = s->allowance;
                s_json["effective_date"] = s->effective_date;
                s_json["created_at"] = s->created_at;

                return crow::response(200, s_json);
            } catch (const std::exception& e) {
                return crow::response(500, "Internal Server Error");
            }
        });

        // 3. Endpoint: Lấy lịch sử điều chỉnh lương của 1 nhân viên (GET /api/salaries/employee/<int>)
        CROW_ROUTE(app, "/api/salaries/employee/<int>").methods(crow::HTTPMethod::GET)
        ([this](const crow::request& req, int employee_id) {
            if (!verify_rbac_permission(req, "READ_SALARY")) {
                return crow::response(403, "Forbidden");
            }

            try {
                auto history = service_->get_salary_history_by_employee(employee_id);
                crow::json::wvalue response_json;
                int index = 0;
                for (const auto& s : history) {
                    response_json[index]["id"] = s.id;
                    response_json[index]["employee_id"] = s.employee_id;
                    response_json[index]["basic_salary"] = s.basic_salary;
                    response_json[index]["allowance"] = s.allowance;
                    response_json[index]["effective_date"] = s.effective_date;
                    response_json[index]["created_at"] = s.created_at;
                    index++;
                }
                return crow::response(200, response_json);
            } catch (const std::exception& e) {
                return crow::response(500, "Internal Server Error");
            }
        });

        // 4. Endpoint: Điều chỉnh mức lương mới cho nhân viên (POST /api/salaries)
        CROW_ROUTE(app, "/api/salaries").methods(crow::HTTPMethod::POST)
        ([this](const crow::request& req) {
            if (!verify_rbac_permission(req, "WRITE_SALARY")) {
                return crow::response(403, "Forbidden");
            }

            try {
                auto body_json = crow::json::load(req.body);
                if (!body_json) {
                    return crow::response(400, "Invalid JSON payload.");
                }

                SalaryCreateDTO dto;
                dto.employee_id = body_json["employee_id"].i();
                
                // GOTCHA TIỀN TỆ: Đọc từ JSON Client dưới dạng integer (VND/cent)
                dto.basic_salary = body_json["basic_salary"].i();
                dto.allowance = body_json["allowance"].i();
                dto.effective_date = body_json["effective_date"].s();

                auto created = service_->adjust_salary(dto);
                
                // GOTCHA BẢO MẬT: Chỉ log mã ID nhân sự, TUYỆT ĐỐI KHÔNG ghi log số tiền lương cơ bản
                log_secure_action("ADJUST_SALARY", "Salary record adjusted for Employee ID: " + std::to_string(created.employee_id) + " | Record ID: " + std::to_string(created.id));

                crow::json::wvalue response_json;
                response_json["id"] = created.id;
                response_json["employee_id"] = created.employee_id;
                response_json["effective_date"] = created.effective_date;

                return crow::response(201, response_json);
            } catch (const std::invalid_argument& e) {
                return crow::response(400, e.what());
            } catch (const std::exception& e) {
                return crow::response(500, "Internal Server Error");
            }
        });

        // 5. Endpoint: Xóa mềm mức lương (DELETE /api/salaries/<int>)
        CROW_ROUTE(app, "/api/salaries/<int>").methods(crow::HTTPMethod::DELETE)
        ([this](const crow::request& req, int id) {
            if (!verify_rbac_permission(req, "WRITE_SALARY")) {
                return crow::response(403, "Forbidden");
            }

            try {
                bool success = service_->delete_salary(id);
                if (!success) {
                    return crow::response(404, "Salary record not found or already deleted.");
                }

                log_secure_action("DELETE_SALARY", "Salary Record ID " + std::to_string(id) + " soft-deleted successfully.");
                return crow::response(200, "Salary record deleted successfully.");
            } catch (const std::exception& e) {
                return crow::response(500, "Internal Server Error");
            }
        });
    }
};

} // namespace hrms
