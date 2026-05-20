---
name: hrms-frontend-skill
description: 'Sử dụng khi phát triển giao diện người dùng (Frontend) cho dự án HRMS sử dụng React, TypeScript và Vite/Next.js. Skill chứa các tiêu chuẩn thành phần giao diện, cách gọi API và tích hợp kiểm tra mã nguồn.'
---

### Quy trình phát triển Frontend React + TypeScript HRMS

1. **Công nghệ & Cú pháp BẮT BUỘC**:
   - BẮT BUỘC sử dụng React (18+), TypeScript (`.ts`, `.tsx`).
   - BẮT BUỘC khai báo kiểu dữ liệu (Interfaces/Types) rõ ràng cho tất cả Props, State và API Response. TUYỆT ĐỐI KHÔNG dùng kiểu `any`.
   - Đặt tên file: Component dùng `PascalCase` (VD: `LeaveRequestForm.tsx`), custom hooks dùng `camelCase` bắt đầu bằng `use` (VD: `useAuth.ts`).

2. **Thiết kế & Giao diện (Aesthetics & UI)**:
   - BẮT BUỘC tuân thủ hệ màu HSL Dark Mode, sử dụng các thẻ bo góc Glassmorphism (`backdrop-filter`).
   - BẮT BUỘC định nghĩa các biến CSS (CSS Variables) hoặc Tailwind CSS Config chuẩn hóa.
   - BẮT BUỘC sử dụng font chữ hiện đại từ Google Fonts (Inter, Outfit).
   - BẮT BUỘC tích hợp hiệu ứng di chuột (hover state) và các hiệu ứng nhỏ (micro-animations) mượt mà cho các phần tử tương tác.

3. **Xử lý Dữ liệu & Nghiệp vụ (Data Gotchas)**:
   - **Xử lý Tiền tệ**: Tiền lương nhận từ Backend là số nguyên (đơn vị VND nhỏ nhất). BẮT BUỘC định dạng hiển thị thông qua bộ định dạng chuyên dụng (VD: `formatVND(basicSalary)`).
   - **Xử lý Múi giờ**: Dữ liệu ngày công, ngày phép nhận từ cơ sở dữ liệu luôn là UTC ISO-8601. BẮT BUỘC chuyển đổi sang múi giờ địa phương `Asia/Ho_Chi_Minh` (GMT+7) khi hiển thị lên màn hình.
   - **Xử lý dữ liệu Nullable**: Luôn bọc kiểm tra an toàn (`data?.property`) hoặc giá trị fallback (`|| '---'`) cho các trường có thể nhận `null` hoặc `undefined` từ API.

4. **Bảo mật & Nhật ký (Security & Log)**:
   - BẮT BUỘC kiểm tra và chèn mã Authorization Bearer Token vào mọi Request Header gửi đến Backend.
   - BẮT BUỘC kiểm tra quyền hạn trước khi render các tính năng có tính bảo mật cao (RBAC). 
   - TUYỆT ĐỐI KHÔNG ghi log số CCCD (`id_card_number`) hay thông tin lương thô ra màn hình console của trình duyệt.

5. **Đồng bộ Kiểm tra (Linting & Formatting)**:
   - BẮT BUỘC kiểm tra mã nguồn bằng ESLint và Prettier trước khi đẩy code lên repository.
   - Chạy script `/scripts/check-lint.sh` để phát hiện và tự động sửa các lỗi định dạng nhanh chóng.
