#pragma once
#include <string>
#include <optional>
#include <cstdint>

namespace hrms {

struct DepartmentCreateDTO {
    std::string code;             // Ví dụ: DEPT-IT, DEPT-HR
    std::string name;
    std::optional<int64_t> manager_id; // FK -> employees(id) (Có thể NULL)
};

struct DepartmentResponseDTO {
    int64_t id;
    std::string code;
    std::string name;
    std::optional<int64_t> manager_id;
    std::string created_at;
    std::optional<std::string> updated_at;
    std::optional<std::string> deleted_at; // GOTCHA QUAN TRỌNG: Phải bọc std::optional xử lý Xóa mềm
};

} // namespace hrms
