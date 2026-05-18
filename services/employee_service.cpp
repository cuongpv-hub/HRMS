#include "services/employee_service.hpp"
#include <utility>
#include <stdexcept>

namespace hrms {

EmployeeService::EmployeeService(std::shared_ptr<IEmployeeRepository> repo)
    : repository_(std::move(repo)) {}

std::vector<EmployeeResponseDTO> EmployeeService::get_active_employees() {
    return repository_->get_all_active();
}

std::optional<EmployeeResponseDTO> EmployeeService::get_employee_by_id(int64_t id) {
    return repository_->find_by_id(id);
}

EmployeeResponseDTO EmployeeService::create_employee(const EmployeeCreateDTO& payload) {
    // Kiểm tra tính hợp lệ nghiệp vụ
    if (payload.code.empty()) {
        throw std::invalid_argument("Employee code cannot be empty.");
    }
    if (payload.full_name.empty()) {
        throw std::invalid_argument("Employee full name cannot be empty.");
    }
    if (payload.email.empty() || payload.email.find('@') == std::string::npos) {
        throw std::invalid_argument("Invalid email address.");
    }
    if (payload.id_card_number.empty()) {
        throw std::invalid_argument("ID Card Number (CCCD) cannot be empty.");
    }

    return repository_->create(payload);
}

std::optional<EmployeeResponseDTO> EmployeeService::update_employee(int64_t id, const EmployeeCreateDTO& payload) {
    auto existing = repository_->find_by_id(id);
    if (!existing.has_value()) {
        return std::nullopt;
    }

    if (payload.code.empty()) {
        throw std::invalid_argument("Employee code cannot be empty.");
    }
    if (payload.full_name.empty()) {
        throw std::invalid_argument("Employee full name cannot be empty.");
    }
    if (payload.email.empty() || payload.email.find('@') == std::string::npos) {
        throw std::invalid_argument("Invalid email address.");
    }
    if (payload.id_card_number.empty()) {
        throw std::invalid_argument("ID Card Number (CCCD) cannot be empty.");
    }

    return repository_->update(id, payload);
}

bool EmployeeService::delete_employee(int64_t id) {
    auto existing = repository_->find_by_id(id);
    if (!existing.has_value()) {
        return false;
    }

    return repository_->soft_delete(id);
}

} // namespace hrms
