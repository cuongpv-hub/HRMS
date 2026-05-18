# Chỉ thị Định dạng Code C++ - HRMS Project

AI Agent BẮT BUỘC tuân thủ 100% các mệnh lệnh dưới đây khi sinh mã nguồn C++:

---

## 1. Quy tắc Đặt tên (Naming Rules)

*   **Tên File**: BẮT BUỘC dùng `snake_case` (VD: `leave_request_service.hpp`).
*   **Class & Struct**: BẮT BUỘC dùng `PascalCase` (VD: `class ContractController`).
*   **Hàm & Phương thức**: BẮT BUỘC dùng `snake_case` (VD: `get_active_contracts()`).
*   **Biến & Tham số**: BẮT BUỘC dùng `snake_case` (VD: `employee_id`).
*   **Thuộc tính Private của Class**: BẮT BUỘC có dấu gạch dưới ở cuối `_` (VD: `repository_`).
*   **Hằng số**: BẮT BUỘC dùng tiền tố `k` + `PascalCase` (VD: `kMaxLeaveDays`).

---

## 2. Định dạng Code (Formatting Rules)

*   **Thụt lề**: BẮT BUỘC sử dụng đúng **4 khoảng trắng (spaces)**. NGHIÊM CẤM dùng ký tự Tab.
*   **Độ dài dòng**: BẮT BUỘC ngắt dòng nếu vượt quá **100 ký tự**.
*   **Dấu ngoặc nhọn**: BẮT BUỘC dùng phong cách K&R Egyptian Braces (mở ngoặc ở cuối dòng khai báo, đóng ngoặc ở dòng mới).
*   **Header Guard**: BẮT BUỘC khai báo `#pragma once` ở đầu mọi tệp tin `.hpp`.

---

## 3. Quản lý Bộ nhớ & Đối tượng (Memory Rules)

*   **Con trỏ**: TUYỆT ĐỐI KHÔNG dùng `new`/`delete` (con trỏ thô). BẮT BUỘC sử dụng `std::shared_ptr` hoặc `std::unique_ptr`.
*   **Chuyển quyền sở hữu**: BẮT BUỘC dùng `std::move` trong Constructor khi tiếp nhận các Smart Pointers.
*   **Constructor**: BẮT BUỘC khai báo `explicit` đối với tất cả Constructor có 1 tham số.

---

## 4. Tách biệt Interface và Implementation (Anti-Header Pollution)

*   **Phân tách file**: BẮT BUỘC tách biệt hoàn toàn phần khai báo trong file header (`.hpp`) và triển khai logic trong file nguồn (`.cpp`).
*   **Tránh ô nhiễm**: TUYỆT ĐỐI KHÔNG viết trực tiếp code logic nghiệp vụ bên trong file `.hpp` (trừ DTO/Schema header-only).

---

## 5. Thực hành C++17 và Ràng buộc CSDL (C++17 & DB Rules)

*   **Trường Nullable**: BẮT BUỘC sử dụng `std::optional<T>` cho mọi trường có thể nhận giá trị `NULL` trong Cơ sở dữ liệu.
*   **Từ khóa chỉ thị**: 
    *   BẮT BUỘC dùng `const` cho các hàm không làm thay đổi trạng thái đối tượng.
    *   BẮT BUỘC viết tường minh từ khóa `override` đối với tất cả các hàm kế thừa ảo.
*   **Thiết kế Repository**: BẮT BUỘC kế thừa từ một Interface thuần ảo (`Abstract Class`).

---

## 6. Mẫu Chuẩn (C++ Blueprint)

```cpp
#pragma once
#include <memory>
#include <optional>
#include <string>

namespace hrms {

class IExampleRepository {
public:
    virtual ~IExampleRepository() = default;
    virtual std::optional<std::string> get_data_by_id(int64_t id) const = 0;
};

class ExampleService {
private:
    std::shared_ptr<IExampleRepository> repository_;

public:
    explicit ExampleService(std::shared_ptr<IExampleRepository> repo)
        : repository_(std::move(repo)) {}

    std::optional<std::string> fetch_secure_data(int64_t id) const {
        if (id <= 0) {
            return std::nullopt;
        }
        return repository_->get_data_by_id(id);
    }
};

} // namespace hrms
```
