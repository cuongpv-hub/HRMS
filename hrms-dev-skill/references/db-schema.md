# Sơ đồ Lược đồ Cơ sở dữ liệu HRMS (HRMS DB Schema)

Dưới đây là cấu trúc lược đồ cơ sở dữ liệu (Database Schema) dạng SQL DDL của hệ thống HRMS. Agent sử dụng lược đồ này để tra cứu chính xác tên bảng, kiểu dữ liệu, các cột và ràng buộc khóa ngoại (Foreign Keys) để thực hiện viết truy vấn chính xác.

```sql
-- 1. Bảng phòng ban (departments)
CREATE TABLE departments (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    code VARCHAR(20) UNIQUE NOT NULL,     -- Ví dụ: DEPT-IT, DEPT-HR
    name VARCHAR(100) NOT NULL,
    manager_id BIGINT,                    -- FK -> employees(id)
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP NULL,
    deleted_at TIMESTAMP NULL
);

-- 2. Bảng nhân viên (employees)
CREATE TABLE employees (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    code VARCHAR(20) UNIQUE NOT NULL,     -- Ví dụ: HRMS-0012
    full_name VARCHAR(100) NOT NULL,
    email VARCHAR(100) UNIQUE NOT NULL,
    phone VARCHAR(15) NULL,
    id_card_number VARCHAR(20) UNIQUE NOT NULL, -- DỮ LIỆU NHẠY CẢM - KHÔNG LOG
    status VARCHAR(20) NOT NULL DEFAULT 'probation', -- active, probation, suspended, terminated
    department_id BIGINT,                 -- FK -> departments(id)
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP NULL,
    deleted_at TIMESTAMP NULL,
    FOREIGN KEY (department_id) REFERENCES departments(id)
);

-- Thêm ràng buộc khóa ngoại trưởng bộ phận cho bảng departments sau khi bảng employees đã tạo
ALTER TABLE departments ADD CONSTRAINT fk_dept_manager FOREIGN KEY (manager_id) REFERENCES employees(id);

-- 3. Bảng hợp đồng lao động (contracts)
CREATE TABLE contracts (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    employee_id BIGINT NOT NULL,          -- FK -> employees(id)
    contract_number VARCHAR(50) UNIQUE NOT NULL,
    type VARCHAR(30) NOT NULL,            -- probation, fixed_term, indefinite
    start_date DATE NOT NULL,
    end_date DATE NULL,                   -- NULL nếu là không thời hạn
    status VARCHAR(20) NOT NULL DEFAULT 'active', -- active, expired, terminated
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP NULL,
    deleted_at TIMESTAMP NULL,
    FOREIGN KEY (employee_id) REFERENCES employees(id)
);

-- 4. Bảng mức lương (salaries)
CREATE TABLE salaries (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    employee_id BIGINT NOT NULL,          -- FK -> employees(id)
    basic_salary DECIMAL(15,2) NOT NULL,  -- Lương cơ bản (DỮ LIỆU NHẠY CẢM - KHÔNG LOG)
    allowance DECIMAL(15,2) DEFAULT 0.00, -- Phụ cấp (DỮ LIỆU NHẠY CẢM - KHÔNG LOG)
    effective_date DATE NOT NULL,         -- Ngày mức lương có hiệu lực
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP NULL,
    deleted_at TIMESTAMP NULL,
    FOREIGN KEY (employee_id) REFERENCES employees(id)
);

-- 5. Bảng yêu cầu nghỉ phép (leave_requests)
CREATE TABLE leave_requests (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    employee_id BIGINT NOT NULL,          -- FK -> employees(id)
    leave_type VARCHAR(30) NOT NULL,      -- annual, sick, unpaid
    start_time TIMESTAMP NOT NULL,        -- UTC
    end_time TIMESTAMP NOT NULL,          -- UTC
    reason TEXT NULL,
    status VARCHAR(20) NOT NULL DEFAULT 'pending', -- pending, approved, rejected
    approver_id BIGINT NULL,              -- FK -> employees(id) (Người duyệt)
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP NULL,
    deleted_at TIMESTAMP NULL,
    FOREIGN KEY (employee_id) REFERENCES employees(id),
    FOREIGN KEY (approver_id) REFERENCES employees(id)
);
```
