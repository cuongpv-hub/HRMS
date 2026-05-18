#pragma once
#include <memory>
#include <vector>
#include <optional>
#include "repositories/i_contract_repository.hpp"
#include "schemas/contract_schema.hpp"

namespace hrms {

class ContractService {
private:
    std::shared_ptr<IContractRepository> repository_;

public:
    // GOTCHA: Bắt buộc sử dụng Dependency Injection thông qua Constructor sử dụng std::move
    explicit ContractService(std::shared_ptr<IContractRepository> repo);

    // Lấy danh sách hợp đồng đang hoạt động (Đã lọc soft delete)
    std::vector<ContractResponseDTO> get_active_contracts();

    // Lấy chi tiết hợp đồng theo ID
    std::optional<ContractResponseDTO> get_contract_by_id(int64_t id);

    // Nghiệp vụ: Tạo mới hợp đồng
    ContractResponseDTO create_contract(const ContractCreateDTO& payload);

    // Nghiệp vụ: Cập nhật thông tin hợp đồng
    std::optional<ContractResponseDTO> update_contract(int64_t id, const ContractCreateDTO& payload);

    // Nghiệp vụ: Chấm dứt hợp đồng (Xóa mềm)
    bool terminate_contract(int64_t id);
};

} // namespace hrms

