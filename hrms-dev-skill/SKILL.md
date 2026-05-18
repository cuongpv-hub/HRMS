---
name: hrms-dev-skill
description: 'Sử dụng khi lập trình, gỡ lỗi, hoặc tái cấu trúc các module của hệ thống Quản lý Nhân sự (HRMS). Skill chứa các quy tắc kiến trúc nội bộ, bảo mật dữ liệu và biểu mẫu code. Không dùng cho dự án ngoài HRMS.'
---

### Quy trình phát triển tính năng HRMS
1. Luôn ưu tiên áp dụng Clean Architecture: Tách biệt rõ ràng Repository (truy xuất dữ liệu) và Controller (logic nghiệp vụ).
2. Kiểm tra tài liệu cơ sở dữ liệu: Trước khi viết bất kỳ câu query nào, hãy đọc tệp `references/db-schema.md` để đảm bảo sử dụng đúng các trường dữ liệu cập nhật nhất.
3. Nếu tính năng liên quan đến luồng khởi tạo đối tượng phức tạp, bắt buộc áp dụng Factory hoặc Builder Pattern.
4. Định dạng đầu ra: Sao chép chính xác cấu trúc từ tệp `assets/code-template.md` khi tạo mới một Controller. Không tự ý sáng tạo cấu trúc file mới.

### Quy tắc Bảo mật
* Mọi API endpoint thay đổi dữ liệu nhân viên bắt buộc phải có middleware kiểm tra quyền truy cập (RBAC).
* Tuyệt đối không in ra console (log) các dữ liệu nhạy cảm của nhân sự (như lương, số CCCD) dưới mọi hình thức.

### Những lưu ý cực kỳ quan trọng (Gotchas)
* Xóa mềm (Soft Deletes): Bảng dữ liệu nhân viên không bao giờ xóa cứng. Luôn phải thêm điều kiện `WHERE deleted_at IS NULL` vào mọi truy vấn.
* Xử lý Tiền tệ (Currency): Tuyệt đối KHÔNG sử dụng kiểu `float` hay `double` để tính toán lương, thưởng nhằm tránh sai số thập phân. Bắt buộc sử dụng kiểu `Decimal` hoặc `Integer` (lưu trữ dưới dạng cents/đồng nhỏ nhất).
* Xử lý Múi giờ (Timezone): Mọi dữ liệu về ngày công, ngày phép phải được lưu dưới dạng `UTC` trong cơ sở dữ liệu. Chỉ chuyển đổi sang giờ địa phương (VD: `GMT+7`) ở lớp hiển thị (Frontend/Presentation layer).
