#include "services/department_service.hpp"
#include <utility>
#include <stdexcept>

namespace hrms {

DepartmentService::DepartmentService(std::shared_ptr<IDepartmentRepository> repo)
    : repository_(std::move(repo)) {}

std::vector<DepartmentResponseDTO> DepartmentService::get_active_departments() {
    return repository_->get_all_active();
}

std::optional<DepartmentResponseDTO> DepartmentService::get_department_by_id(int64_t id) {
    return repository_->find_by_id(id);
}

DepartmentResponseDTO DepartmentService::create_department(const DepartmentCreateDTO& payload) {
    // Kiểm tra tính hợp lệ nghiệp vụ
    if (payload.code.empty()) {
        throw std::invalid_argument("Department code cannot be empty.");
    }
    if (payload.name.empty()) {
        throw std::invalid_argument("Department name cannot be empty.");
    }

    return repository_->create(payload);
}

std::optional<DepartmentResponseDTO> DepartmentService::update_department(int64_t id, const DepartmentCreateDTO& payload) {
    auto existing = repository_->find_by_id(id);
    if (!existing.has_value()) {
        return std::nullopt;
    }

    if (payload.code.empty()) {
        throw std::invalid_argument("Department code cannot be empty.");
    }
    if (payload.name.empty()) {
        throw std::invalid_argument("Department name cannot be empty.");
    }

    return repository_->update(id, payload);
}

bool DepartmentService::delete_department(int64_t id) {
    auto existing = repository_->find_by_id(id);
    if (!existing.has_value()) {
        return false;
    }

    return repository_->soft_delete(id);
}

} // namespace hrms
