#include "services/contract_service.hpp"
#include <utility>
#include <stdexcept>

namespace hrms {

ContractService::ContractService(std::shared_ptr<IContractRepository> repo)
    : repository_(std::move(repo)) {}

std::vector<ContractResponseDTO> ContractService::get_active_contracts() {
    // Gọi repository để lấy tất cả các hợp đồng chưa bị xóa mềm
    return repository_->get_all_active();
}

std::optional<ContractResponseDTO> ContractService::get_contract_by_id(int64_t id) {
    return repository_->find_by_id(id);
}

ContractResponseDTO ContractService::create_contract(const ContractCreateDTO& payload) {
    // 1. Viết logic kiểm tra nghiệp vụ (Business Rules Validation)
    if (payload.contract_number.empty()) {
        throw std::invalid_argument("Contract number cannot be empty.");
    }
    if (payload.employee_id <= 0) {
        throw std::invalid_argument("Invalid employee ID.");
    }
    if (payload.type != "probation" && payload.type != "fixed_term" && payload.type != "indefinite") {
        throw std::invalid_argument("Invalid contract type. Must be probation, fixed_term, or indefinite.");
    }

    // 2. Gọi repository thực hiện chèn dữ liệu
    return repository_->create(payload);
}

std::optional<ContractResponseDTO> ContractService::update_contract(int64_t id, const ContractCreateDTO& payload) {
    // Kiểm tra hợp đồng tồn tại
    auto existing = repository_->find_by_id(id);
    if (!existing) {
        return std::nullopt;
    }

    // Thực hiện cập nhật thông tin thông qua Repository
    return repository_->update(id, payload);
}

bool ContractService::terminate_contract(int64_t id) {
    // Kiểm tra hợp đồng trước khi thực thi chấm dứt
    auto existing = repository_->find_by_id(id);
    if (!existing) {
        return false;
    }

    // GOTCHA: Soft Delete. Repository sẽ thực hiện cập nhật trường deleted_at
    return repository_->soft_delete(id);
}

} // namespace hrms

