#pragma once
#include <memory>
#include <vector>
#include <optional>
#include "repositories/i_leave_request_repository.hpp"
#include "schemas/leave_request_schema.hpp"

namespace hrms {

class LeaveRequestService {
private:
    std::shared_ptr<ILeaveRequestRepository> repository_;

public:
    // GOTCHA: Constructor BẮT BUỘC áp dụng Dependency Injection sử dụng std::move và explicit
    explicit LeaveRequestService(std::shared_ptr<ILeaveRequestRepository> repo);

    // Lấy danh sách yêu cầu nghỉ phép đang hoạt động (Chưa bị xóa mềm)
    std::vector<LeaveRequestResponseDTO> get_active_requests();

    // Lấy chi tiết yêu cầu nghỉ phép theo ID
    std::optional<LeaveRequestResponseDTO> get_request_by_id(int64_t id);

    // Đăng ký yêu cầu nghỉ phép mới
    LeaveRequestResponseDTO submit_request(const LeaveRequestCreateDTO& payload);

    // Phê duyệt yêu cầu nghỉ phép
    std::optional<LeaveRequestResponseDTO> approve_request(int64_t id, int64_t approver_id);

    // Từ chối yêu cầu nghỉ phép
    std::optional<LeaveRequestResponseDTO> reject_request(int64_t id, int64_t approver_id);

    // Hủy bỏ yêu cầu nghỉ phép (Xóa mềm)
    bool cancel_request(int64_t id);
};

} // namespace hrms
