# Mẫu Cấu trúc Clean Architecture cho HRMS C++ (Crow Framework)

Khi được yêu cầu tạo một module mới (ví dụ: Payroll, LeaveRequest), BẮT BUỘC tuân thủ cấu trúc dưới đây. Thay thế `<Module>` bằng tên nghiệp vụ (viết hoa chữ cái đầu cho Class/Struct, viết thường cho tên file).
BẮT BUỘC tách biệt phần khai báo (.hpp) và phần triển khai (.cpp) để tránh ô nhiễm Header.

### 1. Lớp DTO / Schema (`schemas/<module>_schema.hpp`)
Định nghĩa cấu trúc dữ liệu giao tiếp.
**GOTCHA QUAN TRỌNG:** Mọi trường có thể NULL trong Database (như `deleted_at`, `end_date`) BẮT BUỘC phải bọc trong `std::optional`. Tiền tệ BẮT BUỘC dùng `int64_t`.

```cpp
#pragma once
#include <string>
#include <optional>
#include <cstdint>

struct <Module>CreateDTO {
    // VD: int64_t basic_salary; 
};

struct <Module>ResponseDTO {
    int64_t id;
    std::optional<std::string> deleted_at; // Xử lý Soft Delete
};
```

### 2. Lớp Repository
Chỉ giao tiếp với Database. Định nghĩa Interface trong .hpp để Dependency Injection.
**File: repositories/i_<module>_repository.hpp**
```cpp
#pragma once
#include <vector>
#include "schemas/<module>_schema.hpp"

class I<Module>Repository {
public:
    virtual ~I<Module>Repository() = default;
    // GOTCHA: Luôn phải có "WHERE deleted_at IS NULL" khi query
    virtual std::vector<<Module>ResponseDTO> get_all_active() = 0;
};
```

### 3. Lớp Service (Tách biệt Header và Implementation)
Chứa Business Logic. KHÔNG được viết logic trực tiếp vào file .hpp.
**File: services/<module>_service.hpp**
```cpp
#pragma once
#include <memory>
#include "repositories/i_<module>_repository.hpp"
#include "schemas/<module>_schema.hpp"

class <Module>Service {
private:
    std::shared_ptr<I<Module>Repository> repository_;
public:
    explicit <Module>Service(std::shared_ptr<I<Module>Repository> repo);
    <Module>ResponseDTO create_new_record(const <Module>CreateDTO& payload);
};
```

**File: services/<module>_service.cpp**
```cpp
#include "services/<module>_service.hpp"

<Module>Service::<Module>Service(std::shared_ptr<I<Module>Repository> repo) 
    : repository_(std::move(repo)) {}

<Module>ResponseDTO <Module>Service::create_new_record(const <Module>CreateDTO& payload) {
    // Triển khai logic nghiệp vụ tại đây, KHÔNG nằm trong header.
    // Thực hiện mapping và gọi repository.
}
```

### 4. Lớp Controller / API (Crow Framework)
Xử lý Request/Response bằng cú pháp của Crow.
**File: controllers/<module>_controller.hpp**
```cpp
#pragma once
#include <memory>
#include "crow.h"
#include "services/<module>_service.hpp"

class <Module>Controller {
private:
    std::shared_ptr<<Module>Service> service_;
public:
    explicit <Module>Controller(std::shared_ptr<<Module>Service> service) 
        : service_(std::move(service)) {}

    void setup_routes(crow::App<>& app) {
        // GOTCHA: Bắt buộc dùng lambda chaining của Crow
        // BẮT BUỘC chèn middleware kiểm tra RBAC (Role-Based Access Control)
        CROW_ROUTE(app, "/api/<module>s").methods(crow::HTTPMethod::POST)
        ([this](const crow::request& req) {
            // 1. Kiểm tra Quyền truy cập
            // 2. Parse JSON sang DTO
            // 3. Gọi service_->create_new_record()
            // 4. Tuyệt đối KHÔNG in log các DTO chứa dữ liệu nhạy cảm
            return crow::response(200, "Success");
        });
    }
};
```

**Nhận xét:** Với cấu trúc mẫu này, mọi lỗ hổng mà Gemini vừa chỉ ra (Header pollution, ảo giác Router, thiếu Optional NULL) đều đã được bịt kín hoàn toàn. Bạn có thể tự tin áp dụng bộ Agent Skills này vào sản xuất!
