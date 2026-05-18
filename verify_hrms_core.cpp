#include <iostream>
#include <cassert>
#include <memory>
#include <vector>
#include <optional>
#include <stdexcept>

// Include core schemas, repositories and services
#include "repositories/mock_repositories.hpp"
#include "services/department_service.hpp"
#include "services/employee_service.hpp"
#include "services/contract_service.hpp"
#include "services/salary_service.hpp"
#include "services/leave_request_service.hpp"

// Hàm kiểm thử Module Department
void test_department_module() {
    std::cout << "[TEST] Bắt đầu kiểm thử Module Department..." << std::endl;

    auto repo = std::make_shared<hrms::MockDepartmentRepository>();
    hrms::DepartmentService service(repo);

    // 1. Kiểm tra danh sách ban đầu (Có 2 phòng ban IT và HR gieo sẵn)
    auto active_depts = service.get_active_departments();
    assert(active_depts.size() == 2);
    assert(active_depts[0].code == "DEPT-IT");
    assert(active_depts[1].code == "DEPT-HR");

    // 2. Tạo phòng ban mới
    hrms::DepartmentCreateDTO new_dept{"DEPT-SALES", "Sales Department", std::nullopt};
    auto created = service.create_department(new_dept);
    assert(created.id > 0);
    assert(created.code == "DEPT-SALES");

    // 3. Kiểm tra số lượng tăng lên 3
    assert(service.get_active_departments().size() == 3);

    // 4. Xóa mềm phòng ban vừa tạo
    bool deleted = service.delete_department(created.id);
    assert(deleted == true);

    // 5. Kiểm tra phòng ban đã bị ẩn khỏi danh sách hoạt động (Soft delete)
    assert(service.get_active_departments().size() == 2);
    assert(!service.get_department_by_id(created.id).has_value());

    std::cout << "[SUCCESS] Module Department hoạt động hoàn hảo!" << std::endl;
}

// Hàm kiểm thử Module Employee
void test_employee_module() {
    std::cout << "[TEST] Bắt đầu kiểm thử Module Employee..." << std::endl;

    auto repo = std::make_shared<hrms::MockEmployeeRepository>();
    hrms::EmployeeService service(repo);

    // 1. Kiểm tra nhân viên gieo sẵn
    auto active_emps = service.get_active_employees();
    assert(active_emps.size() == 1);
    assert(active_emps[0].full_name == "Pham Minh Cuong");
    assert(active_emps[0].id_card_number == "CCCD-012345");

    // 2. Thử tạo nhân viên mới với email sai định dạng (Kỳ vọng ném ra ngoại lệ)
    hrms::EmployeeCreateDTO invalid_emp{"HRMS-0002", "Invalid User", "invalid_email", "09999", "CCCD-9999", "active", 1};
    try {
        service.create_employee(invalid_emp);
        assert(false); // Không được chạy vào đây
    } catch (const std::invalid_argument& e) {
        std::cout << "  -> Bắt được ngoại lệ email hợp lệ như kỳ vọng: " << e.what() << std::endl;
    }

    // 3. Tạo nhân viên hợp lệ
    hrms::EmployeeCreateDTO valid_emp{"HRMS-0002", "Nguyen Van A", "a.nguyen@hrms.com", "0912345678", "CCCD-22222", "active", 1};
    auto created = service.create_employee(valid_emp);
    assert(created.id > 0);
    assert(created.full_name == "Nguyen Van A");

    // 4. Lấy chi tiết nhân viên
    auto fetched = service.get_employee_by_id(created.id);
    assert(fetched.has_value());
    assert(fetched->code == "HRMS-0002");

    std::cout << "[SUCCESS] Module Employee hoạt động hoàn hảo!" << std::endl;
}

// Hàm kiểm thử Module Contract
void test_contract_module() {
    std::cout << "[TEST] Bắt đầu kiểm thử Module Contract..." << std::endl;

    auto repo = std::make_shared<hrms::MockContractRepository>();
    hrms::ContractService service(repo);

    // 1. Kiểm tra hợp đồng gieo sẵn
    auto active_contracts = service.get_active_contracts();
    assert(active_contracts.size() == 1);
    assert(active_contracts[0].contract_number == "CONTRACT-0001");

    // 2. Tạo hợp đồng mới
    hrms::ContractCreateDTO new_contract{2, "CONTRACT-0002", "probation", "2026-05-18", "2026-07-18"};
    auto created = service.create_contract(new_contract);
    assert(created.id > 0);
    assert(created.contract_number == "CONTRACT-0002");

    std::cout << "[SUCCESS] Module Contract hoạt động hoàn hảo!" << std::endl;
}

// Hàm kiểm thử Module Salary
void test_salary_module() {
    std::cout << "[TEST] Bắt đầu kiểm thử Module Salary..." << std::endl;

    auto repo = std::make_shared<hrms::MockSalaryRepository>();
    hrms::SalaryService service(repo);

    // 1. Kiểm tra mức lương gieo sẵn
    auto active_salaries = service.get_active_salaries();
    assert(active_salaries.size() == 1);
    // GOTCHA TIỀN TỆ: basic_salary là kiểu int64_t lưu đơn vị nhỏ nhất (VND)
    assert(active_salaries[0].basic_salary == 15000000);
    assert(active_salaries[0].allowance == 2000000);

    // 2. Điều chỉnh mức lương mới
    hrms::SalaryCreateDTO new_salary{1, 18000000, 3000000, "2026-06-01"};
    auto created = service.adjust_salary(new_salary);
    assert(created.id > 0);
    assert(created.basic_salary == 18000000);

    // 3. Kiểm tra lịch sử lương của nhân viên ID = 1 (Phải có 2 bản ghi lịch sử)
    auto history = service.get_salary_history_by_employee(1);
    assert(history.size() == 2);

    std::cout << "[SUCCESS] Module Salary hoạt động hoàn hảo!" << std::endl;
}

// Hàm kiểm thử Module LeaveRequest
void test_leave_request_module() {
    std::cout << "[TEST] Bắt đầu kiểm thử Module LeaveRequest..." << std::endl;

    auto repo = std::make_shared<hrms::MockLeaveRequestRepository>();
    hrms::LeaveRequestService service(repo);

    // 1. Kiểm tra yêu cầu nghỉ phép gieo sẵn
    auto active_requests = service.get_active_requests();
    assert(active_requests.size() == 1);
    assert(active_requests[0].status == "pending");

    // 2. Duyệt yêu cầu nghỉ phép (ID = 1)
    auto approved = service.approve_request(1, 999); // Approver ID = 999
    assert(approved.has_value());
    assert(approved->status == "approved");
    assert(approved->approver_id.has_value() && *approved->approver_id == 999);

    // 3. Thử duyệt lại yêu cầu đã duyệt (Kỳ vọng ném ra ngoại lệ vì status không còn là pending)
    try {
        service.approve_request(1, 999);
        assert(false);
    } catch (const std::runtime_error& e) {
        std::cout << "  -> Bắt được ngoại lệ chặn duyệt lại đơn như kỳ vọng: " << e.what() << std::endl;
    }

    std::cout << "[SUCCESS] Module LeaveRequest hoạt động hoàn hảo!" << std::endl;
}

int main() {
    std::cout << "====================================================" << std::endl;
    std::cout << "     BẮT ĐẦU CHẠY SUITE KIỂM THỬ CORE HRMS C++" << std::endl;
    std::cout << "====================================================" << std::endl;

    try {
        test_department_module();
        test_employee_module();
        test_contract_module();
        test_salary_module();
        test_leave_request_module();

        std::cout << "====================================================" << std::endl;
        std::cout << "  🎉 TẤT CẢ CÁC BÀI KIỂM THỬ ĐÃ ĐẠT TRẠNG THÁI SUCCESS!" << std::endl;
        std::cout << "  MÃ NGUỒN HOẠT ĐỘNG HOÀN TOÀN CHÍNH XÁC NHƯ THIẾT KẾ." << std::endl;
        std::cout << "====================================================" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "❌ Kiểm thử thất bại với lỗi: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
