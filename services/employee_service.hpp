#pragma once
#include <memory>
#include <vector>
#include <optional>
#include "repositories/i_employee_repository.hpp"
#include "schemas/employee_schema.hpp"

namespace hrms {

class EmployeeService {
private:
    std::shared_ptr<IEmployeeRepository> repository_;

public:
    // GOTCHA: Constructor BẮT BUỘC áp dụng Dependency Injection sử dụng std::move và explicit
    explicit EmployeeService(std::shared_ptr<IEmployeeRepository> repo);

    // Lấy danh sách nhân viên đang hoạt động (Đã lọc soft delete)
    std::vector<EmployeeResponseDTO> get_active_employees();

    // Lấy chi tiết nhân viên theo ID
    std::optional<EmployeeResponseDTO> get_employee_by_id(int64_t id);

    // Tạo mới nhân viên
    EmployeeResponseDTO create_employee(const EmployeeCreateDTO& payload);

    // Cập nhật nhân viên
    std::optional<EmployeeResponseDTO> update_employee(int64_t id, const EmployeeCreateDTO& payload);

    // Xóa mềm nhân viên
    bool delete_employee(int64_t id);
};

} // namespace hrms
