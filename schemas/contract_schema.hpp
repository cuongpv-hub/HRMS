#pragma once
#include <string>
#include <optional>
#include <cstdint>

namespace hrms {

// DTO dùng khi tạo mới Hợp đồng
struct ContractCreateDTO {
    int64_t employee_id;
    std::string contract_number;
    std::string type;         // probation, fixed_term, indefinite
    std::string start_date;   // Định dạng UTC ISO-8601 (VD: "2026-05-18T00:00:00Z")
    std::optional<std::string> end_date; // Có thể NULL đối với hợp đồng không thời hạn
    std::string status;       // active, expired, terminated
};

// DTO kết quả trả về của Hợp đồng
struct ContractResponseDTO {
    int64_t id;
    int64_t employee_id;
    std::string contract_number;
    std::string type;
    std::string start_date;
    std::optional<std::string> end_date;
    std::string status;
    std::string created_at;                  // Định dạng UTC ISO-8601
    std::optional<std::string> updated_at;   // Có thể NULL
    std::optional<std::string> deleted_at;   // GOTCHA QUAN TRỌNG: Phải bọc std::optional xử lý Xóa mềm
};

} // namespace hrms

