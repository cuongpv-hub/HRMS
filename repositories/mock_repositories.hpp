#pragma once
#include <vector>
#include <optional>
#include <mutex>
#include "repositories/i_department_repository.hpp"
#include "repositories/i_employee_repository.hpp"
#include "repositories/i_contract_repository.hpp"
#include "repositories/i_salary_repository.hpp"
#include "repositories/i_leave_request_repository.hpp"

namespace hrms {

class MockDepartmentRepository : public IDepartmentRepository {
private:
    std::vector<DepartmentResponseDTO> departments_;
    int64_t next_id_ = 1;
    mutable std::mutex mutex_;

public:
    MockDepartmentRepository();
    std::vector<DepartmentResponseDTO> get_all_active() override;
    std::optional<DepartmentResponseDTO> find_by_id(int64_t id) override;
    DepartmentResponseDTO create(const DepartmentCreateDTO& data) override;
    std::optional<DepartmentResponseDTO> update(int64_t id, const DepartmentCreateDTO& data) override;
    bool soft_delete(int64_t id) override;
};

class MockEmployeeRepository : public IEmployeeRepository {
private:
    std::vector<EmployeeResponseDTO> employees_;
    int64_t next_id_ = 1;
    mutable std::mutex mutex_;

public:
    MockEmployeeRepository();
    std::vector<EmployeeResponseDTO> get_all_active() override;
    std::optional<EmployeeResponseDTO> find_by_id(int64_t id) override;
    EmployeeResponseDTO create(const EmployeeCreateDTO& data) override;
    std::optional<EmployeeResponseDTO> update(int64_t id, const EmployeeCreateDTO& data) override;
    bool soft_delete(int64_t id) override;
};

class MockContractRepository : public IContractRepository {
private:
    std::vector<ContractResponseDTO> contracts_;
    int64_t next_id_ = 1;
    mutable std::mutex mutex_;

public:
    MockContractRepository();
    std::vector<ContractResponseDTO> get_all_active() override;
    std::optional<ContractResponseDTO> find_by_id(int64_t id) override;
    ContractResponseDTO create(const ContractCreateDTO& data) override;
    std::optional<ContractResponseDTO> update(int64_t id, const ContractCreateDTO& data) override;
    bool soft_delete(int64_t id) override;
};

class MockSalaryRepository : public ISalaryRepository {
private:
    std::vector<SalaryResponseDTO> salaries_;
    int64_t next_id_ = 1;
    mutable std::mutex mutex_;

public:
    MockSalaryRepository();
    std::vector<SalaryResponseDTO> get_all_active() override;
    std::optional<SalaryResponseDTO> find_by_id(int64_t id) override;
    std::vector<SalaryResponseDTO> find_by_employee_id(int64_t employee_id) override;
    SalaryResponseDTO create(const SalaryCreateDTO& data) override;
    bool soft_delete(int64_t id) override;
};

class MockLeaveRequestRepository : public ILeaveRequestRepository {
private:
    std::vector<LeaveRequestResponseDTO> leave_requests_;
    int64_t next_id_ = 1;
    mutable std::mutex mutex_;

public:
    MockLeaveRequestRepository();
    std::vector<LeaveRequestResponseDTO> get_all_active() override;
    std::optional<LeaveRequestResponseDTO> find_by_id(int64_t id) override;
    LeaveRequestResponseDTO create(const LeaveRequestCreateDTO& data) override;
    std::optional<LeaveRequestResponseDTO> update_status(int64_t id, const std::string& status, int64_t approver_id) override;
    bool soft_delete(int64_t id) override;
};

} // namespace hrms
