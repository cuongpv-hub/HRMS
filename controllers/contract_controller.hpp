#pragma once
#include <memory>
#include <iostream>
#include <string>
#include <utility>
#include "crow.h"
#include "services/contract_service.hpp"

namespace hrms {

class ContractController {
private:
    std::shared_ptr<ContractService> service_;

    // Helper giả lập kiểm tra quyền truy cập (Role-Based Access Control)
    bool verify_rbac_permission(const crow::request& req, const std::string& required_permission) {
        // Trong hệ thống thực tế, quyền sẽ được trích xuất từ JWT Token trong Headers
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return false;
        }
        // Giả lập: Chấp nhận mọi Token hợp lệ chứa chuỗi permission để minh họa
        return auth_header.find(required_permission) != std::string::npos;
    }

    // Helper ghi log bảo mật - Tuyệt đối không log thông tin nhạy cảm
    void log_secure_action(const std::string& action, const std::string& details) {
        // GOTCHA: Chỉ log hành động chung và ID, tuyệt đối không in log nội dung nhạy cảm của hợp đồng ra console.
        std::cout << "[SECURE LOG] Action: " << action << " | " << details << std::endl;
    }

public:
    // GOTCHA: Constructor áp dụng Dependency Injection sử dụng std::move
    explicit ContractController(std::shared_ptr<ContractService> service)
        : service_(std::move(service)) {}

    void setup_routes(crow::SimpleApp& app) {
        
        // 1. Endpoint: Lấy danh sách hợp đồng hoạt động (GET /api/contracts)
        CROW_ROUTE(app, "/api/contracts").methods(crow::HTTPMethod::GET)
        ([this](const crow::request& req) {
            // Kiểm tra phân quyền RBAC
            if (!verify_rbac_permission(req, "READ_CONTRACT")) {
                return crow::response(403, "Forbidden: Insufficient privileges.");
            }

            try {
                auto contracts = service_->get_active_contracts();
                // Giả định hàm chuyển danh sách sang JSON (Crow hỗ trợ crow::json::wvalue)
                crow::json::wvalue response_json;
                int index = 0;
                for (const auto& c : contracts) {
                    response_json[index]["id"] = c.id;
                    response_json[index]["employee_id"] = c.employee_id;
                    response_json[index]["contract_number"] = c.contract_number;
                    response_json[index]["type"] = c.type;
                    response_json[index]["start_date"] = c.start_date;
                    if (c.end_date.has_value()) {
                        response_json[index]["end_date"] = *c.end_date;
                    } else {
                        response_json[index]["end_date"] = nullptr;
                    }
                    response_json[index]["status"] = c.status;
                    response_json[index]["created_at"] = c.created_at;
                    index++;
                }
                return crow::response(200, response_json);
            } catch (const std::exception& e) {
                return crow::response(500, "Internal Server Error");
            }
        });

        // 2. Endpoint: Lấy chi tiết hợp đồng theo ID (GET /api/contracts/<int>)
        CROW_ROUTE(app, "/api/contracts/<int>").methods(crow::HTTPMethod::GET)
        ([this](const crow::request& req, int id) {
            if (!verify_rbac_permission(req, "READ_CONTRACT")) {
                return crow::response(403, "Forbidden");
            }

            try {
                auto contract = service_->get_contract_by_id(id);
                if (!contract.has_value()) {
                    return crow::response(404, "Contract not found or deleted.");
                }

                crow::json::wvalue c_json;
                c_json["id"] = contract->id;
                c_json["employee_id"] = contract->employee_id;
                c_json["contract_number"] = contract->contract_number;
                c_json["type"] = contract->type;
                c_json["start_date"] = contract->start_date;
                if (contract->end_date.has_value()) {
                    c_json["end_date"] = *contract->end_date;
                }
                c_json["status"] = contract->status;
                c_json["created_at"] = contract->created_at;

                return crow::response(200, c_json);
            } catch (const std::exception& e) {
                return crow::response(500, "Internal Server Error");
            }
        });

        // 3. Endpoint: Tạo mới Hợp đồng (POST /api/contracts)
        CROW_ROUTE(app, "/api/contracts").methods(crow::HTTPMethod::POST)
        ([this](const crow::request& req) {
            // BẮT BUỘC chèn middleware kiểm tra RBAC
            if (!verify_rbac_permission(req, "WRITE_CONTRACT")) {
                return crow::response(403, "Forbidden: Action requires WRITE_CONTRACT permission.");
            }

            try {
                // Parse JSON payload sang CreateDTO
                auto body_json = crow::json::load(req.body);
                if (!body_json) {
                    return crow::response(400, "Invalid JSON payload.");
                }

                ContractCreateDTO dto;
                dto.employee_id = body_json["employee_id"].i();
                dto.contract_number = body_json["contract_number"].s();
                dto.type = body_json["type"].s();
                dto.start_date = body_json["start_date"].s();
                if (body_json.has("end_date") && body_json["end_date"].t() != crow::json::type::Null) {
                    dto.end_date = body_json["end_date"].s();
                }
                dto.status = body_json["status"].s();

                // Gọi Service xử lý nghiệp vụ tạo hợp đồng
                auto created = service_->create_contract(dto);

                // GOTCHA: Tuyệt đối không in log cả payload thô chứa thông tin nhạy cảm của nhân viên
                log_secure_action("CREATE_CONTRACT", "Contract successfully created with Number: " + created.contract_number);

                crow::json::wvalue response_json;
                response_json["id"] = created.id;
                response_json["contract_number"] = created.contract_number;
                response_json["status"] = created.status;

                return crow::response(201, response_json);
            } catch (const std::invalid_argument& e) {
                return crow::response(400, e.what());
            } catch (const std::exception& e) {
                return crow::response(500, "Internal Server Error");
            }
        });

        // 4. Endpoint: Chấm dứt / Xóa mềm Hợp đồng (DELETE /api/contracts/<int>)
        CROW_ROUTE(app, "/api/contracts/<int>").methods(crow::HTTPMethod::DELETE)
        ([this](const crow::request& req, int id) {
            if (!verify_rbac_permission(req, "WRITE_CONTRACT")) {
                return crow::response(403, "Forbidden");
            }

            try {
                // Thực hiện xóa mềm ở Service
                bool success = service_->terminate_contract(id);
                if (!success) {
                    return crow::response(404, "Contract not found or already deleted.");
                }

                log_secure_action("TERMINATE_CONTRACT", "Contract ID " + std::to_string(id) + " soft-deleted successfully.");
                return crow::response(200, "Contract terminated successfully.");
            } catch (const std::exception& e) {
                return crow::response(500, "Internal Server Error");
            }
        });
    }
};

} // namespace hrms

