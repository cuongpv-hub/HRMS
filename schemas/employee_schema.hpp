#pragma once
#include <string>
#include <optional>
#include <cstdint>

namespace hrms {

struct EmployeeCreateDTO {
    std::string code;             // Ví dụ: HRMS-0012
    std::string full_name;
    std::string email;
    std::optional<std::string> phone; // Có thể NULL
    std::string id_card_number;   // GOTCHA BẢO MẬT: CCCD LÀ DỮ LIỆU CỰC KỲ NHẠY CẢM - TUYỆT ĐỐI KHÔNG IN LOG
    std::string status;           // active, probation, suspended, terminated
    std::optional<int64_t> department_id; // FK -> departments(id) (Có thể NULL)
};

struct EmployeeResponseDTO {
    int64_t id;
    std::string code;
    std::string full_name;
    std::string email;
    std::optional<std::string> phone;
    std::string id_card_number;   // GOTCHA BẢO MẬT: CCCD LÀ DỮ LIỆU CỰC KỲ NHẠY CẢM - TUYỆT ĐỐI KHÔNG IN LOG
    std::string status;
    std::optional<int64_t> department_id;
    std::string created_at;
    std::optional<std::string> updated_at;
    std::optional<std::string> deleted_at; // GOTCHA QUAN TRỌNG: Phải bọc std::optional xử lý Xóa mềm
};

} // namespace hrms
