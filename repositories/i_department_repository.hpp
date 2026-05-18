#pragma once
#include <vector>
#include <optional>
#include "schemas/department_schema.hpp"

namespace hrms {

class IDepartmentRepository {
public:
    virtual ~IDepartmentRepository() = default;

    // GOTCHA: Triển khai hàm này BẮT BUỘC có điều kiện "WHERE deleted_at IS NULL" khi truy vấn
    virtual std::vector<DepartmentResponseDTO> get_all_active() = 0;

    // Lấy chi tiết phòng ban theo ID (BẮT BUỘC check deleted_at IS NULL)
    virtual std::optional<DepartmentResponseDTO> find_by_id(int64_t id) = 0;

    // Lưu mới phòng ban
    virtual DepartmentResponseDTO create(const DepartmentCreateDTO& data) = 0;

    // Cập nhật thông tin phòng ban
    virtual std::optional<DepartmentResponseDTO> update(int64_t id, const DepartmentCreateDTO& data) = 0;

    // Xóa mềm phòng ban (Cập nhật trường deleted_at = CURRENT_TIMESTAMP)
    virtual bool soft_delete(int64_t id) = 0;
};

} // namespace hrms
