#pragma once
#include <vector>
#include <optional>
#include "schemas/employee_schema.hpp"

namespace hrms {

class IEmployeeRepository {
public:
    virtual ~IEmployeeRepository() = default;

    // GOTCHA: Triển khai hàm này BẮT BUỘC có điều kiện "WHERE deleted_at IS NULL" khi truy vấn
    virtual std::vector<EmployeeResponseDTO> get_all_active() = 0;

    // Lấy chi tiết nhân viên theo ID (BẮT BUỘC check deleted_at IS NULL)
    virtual std::optional<EmployeeResponseDTO> find_by_id(int64_t id) = 0;

    // Lưu mới nhân viên
    virtual EmployeeResponseDTO create(const EmployeeCreateDTO& data) = 0;

    // Cập nhật thông tin nhân viên
    virtual std::optional<EmployeeResponseDTO> update(int64_t id, const EmployeeCreateDTO& data) = 0;

    // Xóa mềm nhân viên (Cập nhật trường deleted_at = CURRENT_TIMESTAMP)
    virtual bool soft_delete(int64_t id) = 0;
};

} // namespace hrms
