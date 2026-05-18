#pragma once
#include <vector>
#include <optional>
#include "schemas/salary_schema.hpp"

namespace hrms {

class ISalaryRepository {
public:
    virtual ~ISalaryRepository() = default;

    // GOTCHA: Triển khai hàm này BẮT BUỘC có điều kiện "WHERE deleted_at IS NULL" khi truy vấn
    virtual std::vector<SalaryResponseDTO> get_all_active() = 0;

    // Lấy chi tiết mức lương theo ID (BẮT BUỘC check deleted_at IS NULL)
    virtual std::optional<SalaryResponseDTO> find_by_id(int64_t id) = 0;

    // Lấy danh sách lịch sử lương của một nhân viên
    virtual std::vector<SalaryResponseDTO> find_by_employee_id(int64_t employee_id) = 0;

    // Lưu mới mức lương
    virtual SalaryResponseDTO create(const SalaryCreateDTO& data) = 0;

    // Xóa mềm mức lương
    virtual bool soft_delete(int64_t id) = 0;
};

} // namespace hrms
