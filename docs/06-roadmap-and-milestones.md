# 06 — Lộ trình phát triển & Milestones (Roadmap & Milestones)

---

## 1. Phương pháp phát triển: Vertical Slices

Không triển khai theo từng tầng hoàn chỉnh (không viết toàn bộ lexer rồi mới đến parser rồi mới đến storage). Dự án chia thành các lát cắt dọc (Vertical Slices) để từng tính năng hoạt động end-to-end ngay từ sớm.

---

## 2. Danh sách 9 Milestones v0.1

### Milestone 1 — Skeleton (Khung dự án & REPL CLI)
- **Mục tiêu:** Build thành công bằng CMake C++20, REPL CLI chạy được, gõ `.exit` và `.help` hoạt động bình thường.
- **Pipeline:** `main` -> `CLI` -> `DatabaseEngine` (chưa cần xử lý SQL).

### Milestone 2 — SELECT Parsing
- **Mục tiêu:** Hỗ trợ cú pháp `SELECT * FROM users;`.
- **Pipeline:** Chuỗi văn bản -> `Lexer` -> `Tokens` -> `Parser` -> `SelectStatement` (AST).

### Milestone 3 — In-Memory Catalog
- **Mục tiêu:** Hỗ trợ `CREATE TABLE users (id INT, name TEXT);`.
- **Pipeline:** Parser -> `CreateTableStatement` -> `Executor` -> `Catalog` (lưu tạm trong RAM).

### Milestone 4 — In-Memory Rows
- **Mục tiêu:** Hỗ trợ `INSERT INTO users VALUES (1, 'Alice');` và `SELECT * FROM users;`.
- **Pipeline:** Lưu trữ hàng tạm thời trong bộ nhớ RAM (dùng container STL). Hoàn thiện pipeline logic SQL frontend-to-executor.

### Milestone 5 — Row Binary Serialization
- **Mục tiêu:** Đóng gói Row thành bytes nhị phân (`INT` 8-bytes, `TEXT` length-prefixed).
- **Kiểm thử:** Test round-trip: `Row -> Bytes -> Row` khớp dữ liệu 100%.

### Milestone 6 — Pager (4096 bytes)
- **Mục tiêu:** Hiện thực hoá abstraction `Pager` quản lý các trang 4KB.
- **API:** `read_page()`, `write_page()`, `allocate_page()`. Test độc lập với file disk.

### Milestone 7 — Persistent Table Storage
- **Mục tiêu:** Thay thế in-memory rows bằng `StorageEngine` ghi dữ liệu xuống các file `tables/<id>.tbl` qua `Pager`.

### Milestone 8 — Persistent Catalog
- **Mục tiêu:** Lưu và nạp schema từ file `catalog.bin`. Khi khởi động lại ứng dụng, `.tables` và `.schema` vẫn hiển thị đầy đủ bảng cũ.

### Milestone 9 — Integration Freeze & End-to-End Test
- **Mục tiêu:** Chạy kịch bản tích hợp hoàn chỉnh:
  1. Mở database.
  2. Tạo bảng `users`.
  3. Thêm bản ghi `(1, 'Alice')`, `(2, 'Bob')`.
  4. Chạy `SELECT *` kiểm tra kết quả.
  5. Thoát chương trình (`.exit`).
  6. Khởi động lại -> Chạy `SELECT *` -> Kết quả vẫn hiển thị chính xác.

---

## 3. Definition of Done (DoD) cho v0.1
1. **Build:** CMake C++20, không warning nghiêm trọng.
2. **CLI:** REPL prompt hoạt động mượt mà, hỗ trợ đủ 4 lệnh meta: `.exit`, `.help`, `.tables`, `.schema`.
3. **SQL:** Hỗ trợ chính xác `CREATE TABLE`, `INSERT`, `SELECT *` với 2 kiểu dữ liệu `INT`, `TEXT`.
4. **Reliability:** Lệnh sai cú pháp, bảng không tồn tại, sai kiểu dữ liệu đều báo lỗi rõ ràng, **tuyệt đối không crash chương trình**.
5. **Persistence:** Dữ liệu tồn tại nguyên vẹn sau khi khởi động lại chương trình.
