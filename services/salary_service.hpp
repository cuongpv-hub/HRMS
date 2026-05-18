#pragma once
#include <memory>
#include <vector>
#include <optional>
#include "repositories/i_salary_repository.hpp"
#include "schemas/salary_schema.hpp"

namespace hrms {

class SalaryService {
private:
    std::shared_ptr<ISalaryRepository> repository_;

public:
    // GOTCHA: Constructor BẮT BUỘC áp dụng Dependency Injection sử dụng std::move và explicit
    explicit SalaryService(std::shared_ptr<ISalaryRepository> repo);

    // Lấy danh sách mức lương đang hoạt động
    std::vector<SalaryResponseDTO> get_active_salaries();

    // Lấy chi tiết mức lương theo ID
    std::optional<SalaryResponseDTO> get_salary_by_id(int64_t id);

    // Lấy lịch sử lương của nhân viên
    std::vector<SalaryResponseDTO> get_salary_history_by_employee(int64_t employee_id);

    // Tạo mới/Điều chỉnh mức lương mới
    SalaryResponseDTO adjust_salary(const SalaryCreateDTO& payload);

    // Xóa mềm mức lương
    bool delete_salary(int64_t id);
};

} // namespace hrms
