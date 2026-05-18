#include "services/salary_service.hpp"
#include <utility>
#include <stdexcept>

namespace hrms {

SalaryService::SalaryService(std::shared_ptr<ISalaryRepository> repo)
    : repository_(std::move(repo)) {}

std::vector<SalaryResponseDTO> SalaryService::get_active_salaries() {
    return repository_->get_all_active();
}

std::optional<SalaryResponseDTO> SalaryService::get_salary_by_id(int64_t id) {
    return repository_->find_by_id(id);
}

std::vector<SalaryResponseDTO> SalaryService::get_salary_history_by_employee(int64_t employee_id) {
    if (employee_id <= 0) {
        throw std::invalid_argument("Invalid employee ID.");
    }
    return repository_->find_by_employee_id(employee_id);
}

SalaryResponseDTO SalaryService::adjust_salary(const SalaryCreateDTO& payload) {
    // Kiểm tra tính hợp lệ nghiệp vụ
    if (payload.employee_id <= 0) {
        throw std::invalid_argument("Invalid employee ID.");
    }
    if (payload.basic_salary < 0) {
        throw std::invalid_argument("Basic salary cannot be negative.");
    }
    if (payload.allowance < 0) {
        throw std::invalid_argument("Allowance cannot be negative.");
    }
    if (payload.effective_date.empty()) {
        throw std::invalid_argument("Effective date cannot be empty.");
    }

    return repository_->create(payload);
}

bool SalaryService::delete_salary(int64_t id) {
    auto existing = repository_->find_by_id(id);
    if (!existing.has_value()) {
        return false;
    }

    return repository_->soft_delete(id);
}

} // namespace hrms
