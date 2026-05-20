# Khuôn mẫu Frontend Chuẩn - HRMS Project

Dưới đây là mã nguồn biểu mẫu chuẩn cho các trang giao diện HRMS sử dụng HTML, Javascript ES6+ và Vanilla CSS. Bản mẫu này tích hợp sẵn giao diện tối (Dark Mode), Glassmorphism, định dạng tiền tệ và múi giờ an toàn.

---

## 1. Tệp tin Giao diện (`index.html`)

```html
<!DOCTYPE html>
<html lang="vi">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Quản lý Nhân sự - HRMS Dashboard</title>
    <meta name="description" content="Hệ thống quản lý thông tin nhân sự và tiền lương nội bộ HRMS.">
    <!-- Google Fonts -->
    <link rel="preconnect" href="https://fonts.googleapis.com">
    <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
    <link href="https://fonts.googleapis.com/css2?family=Outfit:wght@300;400;600;800&display=swap" rel="stylesheet">
    <link rel="stylesheet" href="style.css">
</head>
<body>
    <div class="app-container">
        <header class="app-header">
            <h1 id="main-heading">Hệ thống HRMS</h1>
            <div class="user-profile">
                <span id="user-role-badge" class="badge">Quyền: Quản trị viên</span>
            </div>
        </header>

        <main class="app-main">
            <!-- Khu vực danh sách nhân viên -->
            <section class="card glassmorphism" id="employee-section">
                <h2>Danh sách nhân viên</h2>
                <div class="table-responsive">
                    <table class="data-table" id="employee-table">
                        <thead>
                            <tr>
                                <th>Mã NV</th>
                                <th>Họ và Tên</th>
                                <th>Phòng ban</th>
                                <th>Ngày tham gia</th>
                                <th>Hành động</th>
                            </tr>
                        </thead>
                        <tbody id="employee-table-body">
                            <!-- Dữ liệu render động từ Javascript -->
                        </tbody>
                    </table>
                </div>
            </section>
        </main>
    </div>
    <script src="app.js"></script>
</body>
</html>
```

---

## 2. Phong cách Thiết kế CSS (`style.css`)

```css
/* ==========================================
   HRMS Premium CSS Design System (Dark Mode)
   ========================================== */

:root {
    --font-primary: 'Outfit', sans-serif;
    
    /* HSL Color System */
    --bg-dark: hsl(220, 20%, 8%);
    --bg-card: hsla(220, 20%, 14%, 0.6);
    --border-color: hsla(220, 20%, 25%, 0.5);
    
    --primary: hsl(250, 85%, 65%);
    --primary-hover: hsl(250, 85%, 75%);
    
    --success: hsl(140, 75%, 45%);
    --warning: hsl(35, 80%, 55%);
    --danger: hsl(0, 80%, 55%);
    --text-main: hsl(220, 20%, 95%);
    --text-muted: hsl(220, 10%, 70%);
}

* {
    box-sizing: border-box;
    margin: 0;
    padding: 0;
}

body {
    font-family: var(--font-primary);
    background-color: var(--bg-dark);
    color: var(--text-main);
    line-height: 1.6;
    overflow-x: hidden;
}

.app-container {
    max-width: 1200px;
    margin: 0 auto;
    padding: 2rem;
}

/* Glassmorphism Effect */
.glassmorphism {
    background: var(--bg-card);
    backdrop-filter: blur(12px);
    -webkit-backdrop-filter: blur(12px);
    border: 1px solid var(--border-color);
    border-radius: 16px;
    box-shadow: 0 8px 32px 0 rgba(0, 0, 0, 0.37);
}

.card {
    padding: 2rem;
    margin-bottom: 2rem;
    transition: transform 0.3s ease, border-color 0.3s ease;
}

.card:hover {
    transform: translateY(-4px);
    border-color: var(--primary);
}

/* Premium Buttons with Micro-animations */
.btn {
    font-family: var(--font-primary);
    font-weight: 600;
    padding: 0.6rem 1.2rem;
    border: none;
    border-radius: 8px;
    cursor: pointer;
    transition: all 0.2s cubic-bezier(0.4, 0, 0.2, 1);
    display: inline-flex;
    align-items: center;
    justify-content: center;
    gap: 0.5rem;
}

.btn-primary {
    background-color: var(--primary);
    color: #fff;
}

.btn-primary:hover {
    background-color: var(--primary-hover);
    transform: scale(1.02);
}

.btn-primary:active {
    transform: scale(0.98);
}
```

---

## 3. Logic Xử lý Javascript (`app.js`)

```javascript
/**
 * HRMS Giao tiếp API & Định dạng Dữ liệu
 */
const API_BASE_URL = 'http://localhost:18080/api';
const MOCK_TOKEN = 'READ_EMPLOYEE WRITE_EMPLOYEE READ_SALARY'; // Quyền giả lập

// Helper gọi API an toàn tích hợp RBAC
async function fetchFromBackend(endpoint, options = {}) {
    const headers = {
        'Content-Type': 'application/json',
        'Authorization': `Bearer ${MOCK_TOKEN}`,
        ...(options.headers || {})
    };

    const response = await fetch(`${API_BASE_URL}${endpoint}`, {
        ...options,
        headers
    });

    if (!response.ok) {
        if (response.status === 403) {
            throw new Error('Bạn không có quyền thực hiện hành động này.');
        }
        throw new Error('Lỗi kết nối máy chủ backend.');
    }

    return response.json();
}

// GOTCHA TIỀN TỆ: Chuyển đổi int64_t từ backend thành hiển thị VND
function formatCurrencyVND(amount) {
    if (amount === null || amount === undefined) return '0đ';
    // amount nhận từ API C++ là số nguyên
    return new Intl.NumberFormat('vi-VN', {
        style: 'currency',
        currency: 'VND'
    }).format(amount);
}

// GOTCHA MÚI GIỜ: Chuyển đổi chuỗi ngày giờ UTC ISO-8601 sang múi giờ địa phương GMT+7
function formatLocalTime(utcString) {
    if (!utcString) return '---';
    const date = new Date(utcString);
    return date.toLocaleString('vi-VN', {
        timeZone: 'Asia/Ho_Chi_Minh',
        year: 'numeric',
        month: '2-digit',
        day: '2-digit',
        hour: '2-digit',
        minute: '2-digit'
    });
}

// Render dữ liệu bảng nhân viên
function renderEmployees(employees) {
    const tableBody = document.getElementById('employee-table-body');
    tableBody.innerHTML = '';

    employees.forEach(emp => {
        // GOTCHA NULLABLE: Kiểm tra thuộc tính nullable (phòng ban có thể NULL)
        const departmentName = emp.department_id ? `Phòng ban #${emp.department_id}` : 'Chưa phân bổ';
        const joinDate = formatLocalTime(emp.created_at);

        const row = document.createElement('tr');
        row.innerHTML = `
            <td>${emp.code}</td>
            <td><strong>${emp.full_name}</strong></td>
            <td>${departmentName}</td>
            <td>${joinDate}</td>
            <td>
                <button class="btn btn-primary" id="btn-edit-${emp.id}" onclick="editEmployee(${emp.id})">Sửa</button>
            </td>
        `;
        tableBody.appendChild(row);
    });
}
```
