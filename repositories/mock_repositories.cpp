#include "repositories/mock_repositories.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>

namespace hrms {

// Helper lấy thời gian UTC hiện tại ở chuẩn ISO-8601 string
static std::string current_utc_time() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&in_time_t), "%Y-%m-%dT%H:%M:%SZ");
    return ss.str();
}

// ==========================================
// MockDepartmentRepository
// ==========================================
MockDepartmentRepository::MockDepartmentRepository() {
    // Gieo dữ liệu phòng ban mặc định
    DepartmentCreateDTO it_dept{"DEPT-IT", "Information Technology", std::nullopt};
    DepartmentCreateDTO hr_dept{"DEPT-HR", "Human Resources", std::nullopt};
    create(it_dept);
    create(hr_dept);
}

std::vector<DepartmentResponseDTO> MockDepartmentRepository::get_all_active() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<DepartmentResponseDTO> active;
    for (const auto& d : departments_) {
        // GOTCHA: Chỉ trả về bản ghi chưa bị xóa mềm
        if (!d.deleted_at.has_value()) {
            active.push_back(d);
        }
    }
    return active;
}

std::optional<DepartmentResponseDTO> MockDepartmentRepository::find_by_id(int64_t id) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto& d : departments_) {
        if (d.id == id && !d.deleted_at.has_value()) {
            return d;
        }
    }
    return std::nullopt;
}

DepartmentResponseDTO MockDepartmentRepository::create(const DepartmentCreateDTO& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    DepartmentResponseDTO d;
    d.id = next_id_++;
    d.code = data.code;
    d.name = data.name;
    d.manager_id = data.manager_id;
    d.created_at = current_utc_time();
    departments_.push_back(d);
    return d;
}

std::optional<DepartmentResponseDTO> MockDepartmentRepository::update(int64_t id, const DepartmentCreateDTO& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& d : departments_) {
        if (d.id == id && !d.deleted_at.has_value()) {
            d.code = data.code;
            d.name = data.name;
            d.manager_id = data.manager_id;
            d.updated_at = current_utc_time();
            return d;
        }
    }
    return std::nullopt;
}

bool MockDepartmentRepository::soft_delete(int64_t id) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& d : departments_) {
        if (d.id == id && !d.deleted_at.has_value()) {
            d.deleted_at = current_utc_time();
            return true;
        }
    }
    return false;
}

// ==========================================
// MockEmployeeRepository
// ==========================================
MockEmployeeRepository::MockEmployeeRepository() {
    EmployeeCreateDTO emp1{"HRMS-0001", "Pham Minh Cuong", "cuong.pm@hrms.com", "0987654321", "CCCD-012345", "active", 1};
    create(emp1);
}

std::vector<EmployeeResponseDTO> MockEmployeeRepository::get_all_active() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<EmployeeResponseDTO> active;
    for (const auto& e : employees_) {
        if (!e.deleted_at.has_value()) {
            active.push_back(e);
        }
    }
    return active;
}

std::optional<EmployeeResponseDTO> MockEmployeeRepository::find_by_id(int64_t id) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto& e : employees_) {
        if (e.id == id && !e.deleted_at.has_value()) {
            return e;
        }
    }
    return std::nullopt;
}

EmployeeResponseDTO MockEmployeeRepository::create(const EmployeeCreateDTO& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    EmployeeResponseDTO e;
    e.id = next_id_++;
    e.code = data.code;
    e.full_name = data.full_name;
    e.email = data.email;
    e.phone = data.phone;
    e.id_card_number = data.id_card_number;
    e.status = data.status;
    e.department_id = data.department_id;
    e.created_at = current_utc_time();
    employees_.push_back(e);
    return e;
}

std::optional<EmployeeResponseDTO> MockEmployeeRepository::update(int64_t id, const EmployeeCreateDTO& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& e : employees_) {
        if (e.id == id && !e.deleted_at.has_value()) {
            e.code = data.code;
            e.full_name = data.full_name;
            e.email = data.email;
            e.phone = data.phone;
            e.id_card_number = data.id_card_number;
            e.status = data.status;
            e.department_id = data.department_id;
            e.updated_at = current_utc_time();
            return e;
        }
    }
    return std::nullopt;
}

bool MockEmployeeRepository::soft_delete(int64_t id) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& e : employees_) {
        if (e.id == id && !e.deleted_at.has_value()) {
            e.deleted_at = current_utc_time();
            return true;
        }
    }
    return false;
}

// ==========================================
// MockContractRepository
// ==========================================
MockContractRepository::MockContractRepository() {
    ContractCreateDTO c1{1, "CONTRACT-0001", "fixed_term", "2026-01-01", "2027-01-01"};
    create(c1);
}

std::vector<ContractResponseDTO> MockContractRepository::get_all_active() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<ContractResponseDTO> active;
    for (const auto& c : contracts_) {
        if (!c.deleted_at.has_value()) {
            active.push_back(c);
        }
    }
    return active;
}

std::optional<ContractResponseDTO> MockContractRepository::find_by_id(int64_t id) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto& c : contracts_) {
        if (c.id == id && !c.deleted_at.has_value()) {
            return c;
        }
    }
    return std::nullopt;
}

ContractResponseDTO MockContractRepository::create(const ContractCreateDTO& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    ContractResponseDTO c;
    c.id = next_id_++;
    c.employee_id = data.employee_id;
    c.contract_number = data.contract_number;
    c.type = data.type;
    c.start_date = data.start_date;
    c.end_date = data.end_date;
    c.status = "active";
    c.created_at = current_utc_time();
    contracts_.push_back(c);
    return c;
}

std::optional<ContractResponseDTO> MockContractRepository::update(int64_t id, const ContractCreateDTO& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& c : contracts_) {
        if (c.id == id && !c.deleted_at.has_value()) {
            c.employee_id = data.employee_id;
            c.contract_number = data.contract_number;
            c.type = data.type;
            c.start_date = data.start_date;
            c.end_date = data.end_date;
            c.status = data.status;
            c.updated_at = current_utc_time();
            return c;
        }
    }
    return std::nullopt;
}

bool MockContractRepository::soft_delete(int64_t id) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& c : contracts_) {
        if (c.id == id && !c.deleted_at.has_value()) {
            c.deleted_at = current_utc_time();
            return true;
        }
    }
    return false;
}

// ==========================================
// MockSalaryRepository
// ==========================================
MockSalaryRepository::MockSalaryRepository() {
    SalaryCreateDTO s1{1, 15000000, 2000000, "2026-01-01"}; // 15 triệu VND và 2 triệu VND phụ cấp
    create(s1);
}

std::vector<SalaryResponseDTO> MockSalaryRepository::get_all_active() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<SalaryResponseDTO> active;
    for (const auto& s : salaries_) {
        if (!s.deleted_at.has_value()) {
            active.push_back(s);
        }
    }
    return active;
}

std::optional<SalaryResponseDTO> MockSalaryRepository::find_by_id(int64_t id) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto& s : salaries_) {
        if (s.id == id && !s.deleted_at.has_value()) {
            return s;
        }
    }
    return std::nullopt;
}

std::vector<SalaryResponseDTO> MockSalaryRepository::find_by_employee_id(int64_t employee_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<SalaryResponseDTO> history;
    for (const auto& s : salaries_) {
        if (s.employee_id == employee_id && !s.deleted_at.has_value()) {
            history.push_back(s);
        }
    }
    return history;
}

SalaryResponseDTO MockSalaryRepository::create(const SalaryCreateDTO& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    SalaryResponseDTO s;
    s.id = next_id_++;
    s.employee_id = data.employee_id;
    s.basic_salary = data.basic_salary;
    s.allowance = data.allowance;
    s.effective_date = data.effective_date;
    s.created_at = current_utc_time();
    salaries_.push_back(s);
    return s;
}

bool MockSalaryRepository::soft_delete(int64_t id) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& s : salaries_) {
        if (s.id == id && !s.deleted_at.has_value()) {
            s.deleted_at = current_utc_time();
            return true;
        }
    }
    return false;
}

// ==========================================
// MockLeaveRequestRepository
// ==========================================
MockLeaveRequestRepository::MockLeaveRequestRepository() {
    LeaveRequestCreateDTO l1{1, "annual", "2026-06-01T08:00:00Z", "2026-06-05T17:00:00Z", "Kỳ nghỉ hè gia đình"};
    create(l1);
}

std::vector<LeaveRequestResponseDTO> MockLeaveRequestRepository::get_all_active() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<LeaveRequestResponseDTO> active;
    for (const auto& l : leave_requests_) {
        if (!l.deleted_at.has_value()) {
            active.push_back(l);
        }
    }
    return active;
}

std::optional<LeaveRequestResponseDTO> MockLeaveRequestRepository::find_by_id(int64_t id) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto& l : leave_requests_) {
        if (l.id == id && !l.deleted_at.has_value()) {
            return l;
        }
    }
    return std::nullopt;
}

LeaveRequestResponseDTO MockLeaveRequestRepository::create(const LeaveRequestCreateDTO& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    LeaveRequestResponseDTO l;
    l.id = next_id_++;
    l.employee_id = data.employee_id;
    l.leave_type = data.leave_type;
    l.start_time = data.start_time;
    l.end_time = data.end_time;
    l.reason = data.reason;
    l.status = "pending";
    l.approver_id = std::nullopt;
    l.created_at = current_utc_time();
    leave_requests_.push_back(l);
    return l;
}

std::optional<LeaveRequestResponseDTO> MockLeaveRequestRepository::update_status(int64_t id, const std::string& status, int64_t approver_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& l : leave_requests_) {
        if (l.id == id && !l.deleted_at.has_value()) {
            l.status = status;
            l.approver_id = approver_id;
            l.updated_at = current_utc_time();
            return l;
        }
    }
    return std::nullopt;
}

bool MockLeaveRequestRepository::soft_delete(int64_t id) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& l : leave_requests_) {
        if (l.id == id && !l.deleted_at.has_value()) {
            l.deleted_at = current_utc_time();
            return true;
        }
    }
    return false;
}

} // namespace hrms
