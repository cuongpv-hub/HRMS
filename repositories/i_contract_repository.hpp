#pragma once
#include <vector>
#include <optional>
#include "schemas/contract_schema.hpp"

namespace hrms {

class IContractRepository {
public:
    virtual ~IContractRepository() = default;

    // GOTCHA: Triển khai hàm này bắt buộc phải áp dụng "WHERE deleted_at IS NULL"
    virtual std::vector<ContractResponseDTO> get_all_active() = 0;

    // Lấy chi tiết hợp đồng theo ID (Cũng bắt buộc check deleted_at IS NULL)
    virtual std::optional<ContractResponseDTO> find_by_id(int64_t id) = 0;

    // Lưu mới hợp đồng vào cơ sở dữ liệu
    virtual ContractResponseDTO create(const ContractCreateDTO& data) = 0;

    // Cập nhật hợp đồng
    virtual std::optional<ContractResponseDTO> update(int64_t id, const ContractCreateDTO& data) = 0;

    // Xóa mềm hợp đồng (Cập nhật trường deleted_at = CURRENT_TIMESTAMP)
    virtual bool soft_delete(int64_t id) = 0;
};

} // namespace hrms

