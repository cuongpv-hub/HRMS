#pragma once
#include <memory>
#include <vector>
#include <optional>
#include "repositories/i_department_repository.hpp"
#include "schemas/department_schema.hpp"

namespace hrms {

class DepartmentService {
private:
    std::shared_ptr<IDepartmentRepository> repository_;

public:
    // GOTCHA: Constructor BẮT BUỘC áp dụng Dependency Injection sử dụng std::move và explicit
    explicit DepartmentService(std::shared_ptr<IDepartmentRepository> repo);

    // Lấy danh sách phòng ban đang hoạt động (Đã lọc soft delete)
    std::vector<DepartmentResponseDTO> get_active_departments();

    // Lấy chi tiết phòng ban theo ID
    std::optional<DepartmentResponseDTO> get_department_by_id(int64_t id);

    // Tạo mới phòng ban
    DepartmentResponseDTO create_department(const DepartmentCreateDTO& payload);

    // Cập nhật phòng ban
    std::optional<DepartmentResponseDTO> update_department(int64_t id, const DepartmentCreateDTO& payload);

    // Xóa mềm phòng ban
    bool delete_department(int64_t id);
};

} // namespace hrms
