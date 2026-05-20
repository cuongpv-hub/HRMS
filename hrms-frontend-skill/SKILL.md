---
name: hrms-frontend-skill
description: 'Sử dụng khi xây dựng, sửa đổi, gỡ lỗi hoặc tối ưu hóa giao diện người dùng (Frontend) của hệ thống HRMS. Hướng dẫn thiết kế giao diện premium, cấu trúc mã nguồn và đồng bộ với REST API.'
---

### Quy trình phát triển Frontend HRMS

1. **Công nghệ cốt lõi**: 
   - BẮT BUỘC sử dụng HTML5 semantic, Javascript ES6+ và Vanilla CSS.
   - Chỉ sử dụng React/Vite/Next.js khi người dùng yêu cầu rõ ràng.
   - TUYỆT ĐỐI KHÔNG dùng TailwindCSS trừ khi có yêu cầu cụ thể kèm phiên bản.

2. **Thiết kế & Giao diện (Aesthetics & UI)**:
   - BẮT BUỘC thiết kế giao diện Premium: Sử dụng bảng màu HSL hài hòa, nền tối (Dark Mode), hiệu ứng mờ kính (Glassmorphism), viền mỏng tinh tế.
   - BẮT BUỘC tích hợp micro-animations và hiệu ứng di chuột (hover effects) mượt mà cho tất cả các nút bấm, thẻ thông tin (card) và ô nhập liệu.
   - BẮT BUỘC sử dụng font chữ hiện đại từ Google Fonts (Inter, Outfit, Roboto).
   - TUYỆT ĐỐI KHÔNG sử dụng ảnh hoặc dữ liệu placeholder trống. BẮT BUỘC tạo hoặc chuẩn bị ảnh giao diện thực tế.

3. **Đồng bộ REST API & Gotchas dữ liệu**:
   - BẮT BUỘC kết nối chính xác cổng `18080` của Backend C++ Crow Server (`http://localhost:18080/api/...`).
   - **Xử lý Tiền tệ (Currency)**: Nhận dữ liệu số nguyên `int64_t` từ API (đơn vị VND nhỏ nhất) và BẮT BUỘC chuyển đổi định dạng hiển thị tiền tệ (VD: `15,000,000đ`) trên giao diện.
   - **Xử lý Múi giờ (Timezone)**: Nhận chuỗi ngày giờ UTC ISO-8601 và BẮT BUỘC định dạng hiển thị sang giờ địa phương `GMT+7` cho người dùng.
   - **Xử lý Nullable**: BẮT BUỘC kiểm tra an toàn dữ liệu nullable (tránh crash ứng dụng khi thuộc tính có giá trị `null` hoặc `undefined`).

4. **Bảo mật & Phân quyền (Security & RBAC)**:
   - BẮT BUỘC đính kèm Authorization header chứa JWT/Token giả lập phân quyền tương ứng khi gọi API (VD: `Authorization: Bearer READ_EMPLOYEE WRITE_EMPLOYEE`).
   - BẮT BUỘC ẩn hoặc vô hiệu hóa (disable) các nút hành động (Thêm, Sửa, Xóa) trên UI nếu người dùng không đủ quyền tương ứng.
   - TUYỆT ĐỐI KHÔNG hiển thị số CCCD (`id_card_number`) hoặc thông tin lương thô ra màn hình console của trình duyệt.

5. **Chuẩn SEO & Khả dụng (SEO & UX)**:
   - BẮT BUỘC cấu trúc đúng thẻ tiêu đề `<h1>` duy nhất cho mỗi trang.
   - BẮT BUỘC khai báo mô tả meta (`meta description`) và tiêu đề trang (`title`) phản ánh đúng tính năng.
   - BẮT BUỘC gán thuộc tính `id` duy nhất và mô tả rõ nghĩa cho tất cả các phần tử tương tác (button, input, form) để phục vụ kiểm thử tự động.
