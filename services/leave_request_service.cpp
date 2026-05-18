#include "services/leave_request_service.hpp"
#include <utility>
#include <stdexcept>

namespace hrms {

LeaveRequestService::LeaveRequestService(std::shared_ptr<ILeaveRequestRepository> repo)
    : repository_(std::move(repo)) {}

std::vector<LeaveRequestResponseDTO> LeaveRequestService::get_active_requests() {
    return repository_->get_all_active();
}

std::optional<LeaveRequestResponseDTO> LeaveRequestService::get_request_by_id(int64_t id) {
    return repository_->find_by_id(id);
}

LeaveRequestResponseDTO LeaveRequestService::submit_request(const LeaveRequestCreateDTO& payload) {
    // 1. Kiểm tra tính hợp lệ của nghiệp vụ (Business Validation)
    if (payload.employee_id <= 0) {
        throw std::invalid_argument("Invalid employee ID.");
    }
    if (payload.leave_type != "annual" && payload.leave_type != "sick" && payload.leave_type != "unpaid") {
        throw std::invalid_argument("Invalid leave type. Must be annual, sick, or unpaid.");
    }
    if (payload.start_time.empty() || payload.end_time.empty()) {
        throw std::invalid_argument("Start time and end time cannot be empty.");
    }

    // 2. Chuyển tiếp lưu trữ qua Repository
    return repository_->create(payload);
}

std::optional<LeaveRequestResponseDTO> LeaveRequestService::approve_request(int64_t id, int64_t approver_id) {
    // 1. Kiểm tra yêu cầu nghỉ phép tồn tại
    auto request = repository_->find_by_id(id);
    if (!request.has_value()) {
        return std::nullopt;
    }

    // 2. Chỉ được phép duyệt khi yêu cầu đang ở trạng thái 'pending'
    if (request->status != "pending") {
        throw std::runtime_error("Only pending requests can be approved.");
    }
    if (approver_id <= 0) {
        throw std::invalid_argument("Invalid approver ID.");
    }

    // 3. Thực hiện cập nhật trạng thái duyệt
    return repository_->update_status(id, "approved", approver_id);
}

std::optional<LeaveRequestResponseDTO> LeaveRequestService::reject_request(int64_t id, int64_t approver_id) {
    auto request = repository_->find_by_id(id);
    if (!request.has_value()) {
        return std::nullopt;
    }

    if (request->status != "pending") {
        throw std::runtime_error("Only pending requests can be rejected.");
    }
    if (approver_id <= 0) {
        throw std::invalid_argument("Invalid approver ID.");
    }

    return repository_->update_status(id, "rejected", approver_id);
}

bool LeaveRequestService::cancel_request(int64_t id) {
    auto request = repository_->find_by_id(id);
    if (!request.has_value()) {
        return false;
    }

    // GOTCHA: Thực hiện Xóa mềm (Soft Delete) chứ không xóa vật lý khỏi bảng
    return repository_->soft_delete(id);
}

} // namespace hrms
