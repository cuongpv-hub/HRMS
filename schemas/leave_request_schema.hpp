#pragma once
#include <string>
#include <optional>
#include <cstdint>

namespace hrms {

struct LeaveRequestCreateDTO {
    int64_t employee_id;
    std::string leave_type;       // annual, sick, unpaid
    std::string start_time;       // Định dạng UTC ISO-8601 (VD: "2026-05-18T08:00:00Z")
    std::string end_time;         // Định dạng UTC ISO-8601
    std::optional<std::string> reason; // Lý do xin nghỉ (Có thể NULL)
};

struct LeaveRequestResponseDTO {
    int64_t id;
    int64_t employee_id;
    std::string leave_type;
    std::string start_time;
    std::string end_time;
    std::optional<std::string> reason;
    std::string status;                     // pending, approved, rejected
    std::optional<int64_t> approver_id;     // Người duyệt (NULL khi chưa duyệt)
    std::string created_at;                 // Định dạng UTC ISO-8601
    std::optional<std::string> updated_at;  // Có thể NULL
    std::optional<std::string> deleted_at;  // GOTCHA QUAN TRỌNG: Phải bọc std::optional xử lý Xóa mềm
};

} // namespace hrms
