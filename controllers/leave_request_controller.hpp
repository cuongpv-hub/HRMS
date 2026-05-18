#pragma once
#include <memory>
#include <iostream>
#include <string>
#include <utility>
#include "crow.h"
#include "services/leave_request_service.hpp"

namespace hrms {

class LeaveRequestController {
private:
    std::shared_ptr<LeaveRequestService> service_;

    // Helper giả lập kiểm tra quyền truy cập (RBAC)
    bool verify_rbac_permission(const crow::request& req, const std::string& required_permission) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return false;
        }
        return auth_header.find(required_permission) != std::string::npos;
    }

    // Helper ghi log bảo mật - Không log thông tin nhạy cảm
    void log_secure_action(const std::string& action, const std::string& details) {
        // GOTCHA: Log an toàn, tuyệt đối không in log lí do xin nghỉ phép hay thông tin cá nhân của nhân viên
        std::cout << "[SECURE LOG] Action: " << action << " | " << details << std::endl;
    }

public:
    // GOTCHA: Constructor áp dụng Dependency Injection sử dụng std::move và explicit
    explicit LeaveRequestController(std::shared_ptr<LeaveRequestService> service)
        : service_(std::move(service)) {}

    void setup_routes(crow::SimpleApp& app) {

        // 1. Endpoint: Lấy danh sách yêu cầu nghỉ phép đang hoạt động (GET /api/leave_requests)
        CROW_ROUTE(app, "/api/leave_requests").methods(crow::HTTPMethod::GET)
        ([this](const crow::request& req) {
            if (!verify_rbac_permission(req, "READ_LEAVE_REQUEST")) {
                return crow::response(403, "Forbidden");
            }

            try {
                auto requests = service_->get_active_requests();
                crow::json::wvalue response_json;
                int index = 0;
                for (const auto& r : requests) {
                    response_json[index]["id"] = r.id;
                    response_json[index]["employee_id"] = r.employee_id;
                    response_json[index]["leave_type"] = r.leave_type;
                    response_json[index]["start_time"] = r.start_time;
                    response_json[index]["end_time"] = r.end_time;
                    if (r.reason.has_value()) {
                        response_json[index]["reason"] = *r.reason;
                    } else {
                        response_json[index]["reason"] = nullptr;
                    }
                    response_json[index]["status"] = r.status;
                    if (r.approver_id.has_value()) {
                        response_json[index]["approver_id"] = *r.approver_id;
                    } else {
                        response_json[index]["approver_id"] = nullptr;
                    }
                    response_json[index]["created_at"] = r.created_at;
                    index++;
                }
                return crow::response(200, response_json);
            } catch (const std::exception& e) {
                return crow::response(500, "Internal Server Error");
            }
        });

        // 2. Endpoint: Lấy chi tiết yêu cầu nghỉ phép theo ID (GET /api/leave_requests/<int>)
        CROW_ROUTE(app, "/api/leave_requests/<int>").methods(crow::HTTPMethod::GET)
        ([this](const crow::request& req, int id) {
            if (!verify_rbac_permission(req, "READ_LEAVE_REQUEST")) {
                return crow::response(403, "Forbidden");
            }

            try {
                auto r = service_->get_request_by_id(id);
                if (!r.has_value()) {
                    return crow::response(404, "Leave request not found or deleted.");
                }

                crow::json::wvalue r_json;
                r_json["id"] = r->id;
                r_json["employee_id"] = r->employee_id;
                r_json["leave_type"] = r->leave_type;
                r_json["start_time"] = r->start_time;
                r_json["end_time"] = r->end_time;
                if (r->reason.has_value()) {
                    r_json["reason"] = *r->reason;
                }
                r_json["status"] = r->status;
                if (r->approver_id.has_value()) {
                    r_json["approver_id"] = *r->approver_id;
                }
                r_json["created_at"] = r->created_at;

                return crow::response(200, r_json);
            } catch (const std::exception& e) {
                return crow::response(500, "Internal Server Error");
            }
        });

        // 3. Endpoint: Nộp đơn xin nghỉ phép mới (POST /api/leave_requests)
        CROW_ROUTE(app, "/api/leave_requests").methods(crow::HTTPMethod::POST)
        ([this](const crow::request& req) {
            if (!verify_rbac_permission(req, "WRITE_LEAVE_REQUEST")) {
                return crow::response(403, "Forbidden");
            }

            try {
                auto body_json = crow::json::load(req.body);
                if (!body_json) {
                    return crow::response(400, "Invalid JSON payload.");
                }

                LeaveRequestCreateDTO dto;
                dto.employee_id = body_json["employee_id"].i();
                dto.leave_type = body_json["leave_type"].s();
                dto.start_time = body_json["start_time"].s();
                dto.end_time = body_json["end_time"].s();
                if (body_json.has("reason") && body_json["reason"].t() != crow::json::type::Null) {
                    dto.reason = body_json["reason"].s();
                }

                auto created = service_->submit_request(dto);
                log_secure_action("SUBMIT_LEAVE_REQUEST", "Leave request submitted for Employee ID: " + std::to_string(created.employee_id));

                crow::json::wvalue response_json;
                response_json["id"] = created.id;
                response_json["status"] = created.status;

                return crow::response(201, response_json);
            } catch (const std::invalid_argument& e) {
                return crow::response(400, e.what());
            } catch (const std::exception& e) {
                return crow::response(500, "Internal Server Error");
            }
        });

        // 4. Endpoint: Phê duyệt nghỉ phép (POST /api/leave_requests/<int>/approve)
        CROW_ROUTE(app, "/api/leave_requests/<int>/approve").methods(crow::HTTPMethod::POST)
        ([this](const crow::request& req, int id) {
            if (!verify_rbac_permission(req, "APPROVE_LEAVE_REQUEST")) {
                return crow::response(403, "Forbidden");
            }

            try {
                auto body_json = crow::json::load(req.body);
                if (!body_json || !body_json.has("approver_id")) {
                    return crow::response(400, "Missing approver_id.");
                }
                int64_t approver_id = body_json["approver_id"].i();

                auto updated = service_->approve_request(id, approver_id);
                if (!updated.has_value()) {
                    return crow::response(404, "Leave request not found or not in pending state.");
                }

                log_secure_action("APPROVE_LEAVE_REQUEST", "Approved Leave Request ID: " + std::to_string(id) + " by Manager: " + std::to_string(approver_id));

                crow::json::wvalue response_json;
                response_json["id"] = updated->id;
                response_json["status"] = updated->status;

                return crow::response(200, response_json);
            } catch (const std::runtime_error& e) {
                return crow::response(400, e.what());
            } catch (const std::exception& e) {
                return crow::response(500, "Internal Server Error");
            }
        });

        // 5. Endpoint: Từ chối nghỉ phép (POST /api/leave_requests/<int>/reject)
        CROW_ROUTE(app, "/api/leave_requests/<int>/reject").methods(crow::HTTPMethod::POST)
        ([this](const crow::request& req, int id) {
            if (!verify_rbac_permission(req, "APPROVE_LEAVE_REQUEST")) {
                return crow::response(403, "Forbidden");
            }

            try {
                auto body_json = crow::json::load(req.body);
                if (!body_json || !body_json.has("approver_id")) {
                    return crow::response(400, "Missing approver_id.");
                }
                int64_t approver_id = body_json["approver_id"].i();

                auto updated = service_->reject_request(id, approver_id);
                if (!updated.has_value()) {
                    return crow::response(404, "Leave request not found or not in pending state.");
                }

                log_secure_action("REJECT_LEAVE_REQUEST", "Rejected Leave Request ID: " + std::to_string(id) + " by Manager: " + std::to_string(approver_id));

                crow::json::wvalue response_json;
                response_json["id"] = updated->id;
                response_json["status"] = updated->status;

                return crow::response(200, response_json);
            } catch (const std::runtime_error& e) {
                return crow::response(400, e.what());
            } catch (const std::exception& e) {
                return crow::response(500, "Internal Server Error");
            }
        });

        // 6. Endpoint: Hủy bỏ yêu cầu nghỉ phép (DELETE /api/leave_requests/<int>)
        CROW_ROUTE(app, "/api/leave_requests/<int>").methods(crow::HTTPMethod::DELETE)
        ([this](const crow::request& req, int id) {
            if (!verify_rbac_permission(req, "WRITE_LEAVE_REQUEST")) {
                return crow::response(403, "Forbidden");
            }

            try {
                bool success = service_->cancel_request(id);
                if (!success) {
                    return crow::response(404, "Leave request not found or already deleted.");
                }

                log_secure_action("CANCEL_LEAVE_REQUEST", "Leave Request ID " + std::to_string(id) + " cancelled (soft-deleted).");
                return crow::response(200, "Leave request cancelled successfully.");
            } catch (const std::exception& e) {
                return crow::response(500, "Internal Server Error");
            }
        });
    }
};

} // namespace hrms
