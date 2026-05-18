#pragma once
#include <vector>
#include <optional>
#include <string>
#include "schemas/leave_request_schema.hpp"

namespace hrms {

class ILeaveRequestRepository {
public:
    virtual ~ILeaveRequestRepository() = default;

    // GOTCHA: Triển khai hàm này BẮT BUỘC có điều kiện "WHERE deleted_at IS NULL" khi truy vấn
    virtual std::vector<LeaveRequestResponseDTO> get_all_active() = 0;

    // Lấy chi tiết yêu cầu nghỉ phép theo ID (BẮT BUỘC check deleted_at IS NULL)
    virtual std::optional<LeaveRequestResponseDTO> find_by_id(int64_t id) = 0;

    // Lưu mới yêu cầu nghỉ phép
    virtual LeaveRequestResponseDTO create(const LeaveRequestCreateDTO& data) = 0;

    // Cập nhật trạng thái duyệt (Status & Approver)
    virtual std::optional<LeaveRequestResponseDTO> update_status(int64_t id, const std::string& status, int64_t approver_id) = 0;

    // Xóa mềm yêu cầu nghỉ phép (deleted_at = CURRENT_TIMESTAMP)
    virtual bool soft_delete(int64_t id) = 0;
};

} // namespace hrms
