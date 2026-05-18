#pragma once
#include <string>
#include <optional>
#include <cstdint>

namespace hrms {

struct SalaryCreateDTO {
    int64_t employee_id;          // FK -> employees(id)
    int64_t basic_salary;         // GOTCHA TIỀN TỆ: BẮT BUỘC dùng int64_t (lưu đơn vị nhỏ nhất như VND/cent) - TUYỆT ĐỐI KHÔNG dùng float/double
    int64_t allowance;            // GOTCHA TIỀN TỆ: BẮT BUỘC dùng int64_t (đơn vị nhỏ nhất)
    std::string effective_date;   // Định dạng UTC ISO-8601 (VD: "2026-05-18")
};

struct SalaryResponseDTO {
    int64_t id;
    int64_t employee_id;
    int64_t basic_salary;         // GOTCHA BẢO MẬT: TIỀN LƯƠNG LÀ DỮ LIỆU CỰC KỲ NHẠY CẢM - TUYỆT ĐỐI KHÔNG IN LOG
    int64_t allowance;            // GOTCHA BẢO MẬT: TUYỆT ĐỐI KHÔNG IN LOG
    std::string effective_date;
    std::string created_at;
    std::optional<std::string> updated_at;
    std::optional<std::string> deleted_at; // GOTCHA QUAN TRỌNG: Phải bọc std::optional xử lý Xóa mềm
};

} // namespace hrms
