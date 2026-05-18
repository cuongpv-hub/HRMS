# Developer Scripts (`/scripts`)

Thư mục này chứa các script tự động hóa, công cụ hỗ trợ phát triển, deploy, database migration, và các tác vụ bảo trì hệ thống.

## 📂 Danh mục các script khuyến nghị
- **Database Utilities**:
  - `migrate`: Chạy các bản cập nhật database.
  - `seed`: Nạp dữ liệu giả lập (mock/seed data) để phục vụ test/dev.
- **Docker / Deployment**:
  - Các shell scripts (`.sh` hoặc `.ps1`) để build, run container cục bộ hoặc môi trường staging.
- **Git Hooks / Linters**:
  - Scripts thiết lập pre-commit hooks, kiểm tra code style tự động trước khi commit.

*Hãy đảm bảo viết tài liệu hướng dẫn sử dụng rõ ràng bên trong mỗi script hoặc cập nhật tại file này.*
