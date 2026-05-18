#include <memory>
#include <iostream>
#include "crow.h"
#include "repositories/mock_repositories.hpp"
#include "services/department_service.hpp"
#include "services/employee_service.hpp"
#include "services/contract_service.hpp"
#include "services/salary_service.hpp"
#include "services/leave_request_service.hpp"
#include "controllers/department_controller.hpp"
#include "controllers/employee_controller.hpp"
#include "controllers/contract_controller.hpp"
#include "controllers/salary_controller.hpp"
#include "controllers/leave_request_controller.hpp"

int main() {
    // 1. Khởi tạo Crow Application
    crow::SimpleApp app;

    // 2. Khởi tạo Concrete Mock Repositories
    auto dept_repo = std::make_shared<hrms::MockDepartmentRepository>();
    auto emp_repo = std::make_shared<hrms::MockEmployeeRepository>();
    auto contract_repo = std::make_shared<hrms::MockContractRepository>();
    auto salary_repo = std::make_shared<hrms::MockSalaryRepository>();
    auto leave_repo = std::make_shared<hrms::MockLeaveRequestRepository>();

    // 3. Khởi tạo Services (Áp dụng Dependency Injection sử dụng Smart Pointers)
    auto dept_service = std::make_shared<hrms::DepartmentService>(dept_repo);
    auto emp_service = std::make_shared<hrms::EmployeeService>(emp_repo);
    auto contract_service = std::make_shared<hrms::ContractService>(contract_repo);
    auto salary_service = std::make_shared<hrms::SalaryService>(salary_repo);
    auto leave_service = std::make_shared<hrms::LeaveRequestService>(leave_repo);

    // 4. Khởi tạo Controllers (Dependency Injection)
    hrms::DepartmentController dept_controller(dept_service);
    hrms::EmployeeController emp_controller(emp_service);
    hrms::ContractController contract_controller(contract_service);
    hrms::SalaryController salary_controller(salary_service);
    hrms::LeaveRequestController leave_controller(leave_service);

    // 5. Đăng ký các API Endpoint định tuyến Crow
    dept_controller.setup_routes(app);
    emp_controller.setup_routes(app);
    contract_controller.setup_routes(app);
    salary_controller.setup_routes(app);
    leave_controller.setup_routes(app);

    // 6. Cấu hình & Khởi chạy C++ REST API Server
    std::cout << "====================================================" << std::endl;
    std::cout << "  HRMS C++ REST API Server is starting on port 18080" << std::endl;
    std::cout << "  Base API Route: http://localhost:18080/api" << std::endl;
    std::cout << "====================================================" << std::endl;

    app.port(18080).multithreaded().run();

    return 0;
}
