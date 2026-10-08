# Milestone 1 — Skeleton (Khung dự án & REPL CLI)

**Mục tiêu chính:** Xây dựng khung ứng dụng cơ bản hoạt động end-to-end từ `main` qua `CLI` đến `DatabaseEngine`, chạy được vòng lặp REPL, xử lý các lệnh meta-commands cơ bản, và biên dịch sạch sẽ bằng CMake C++20. **(Chưa thực thi SQL)**.

---

## 1. Kiến trúc luồng của Milestone 1

```text
main()
  │
  ▼
CLI.run() ◄────────────┐
  │                    │
  ├──► Nhập input      │ (REPL Loop)
  │                    │
  ├──► Meta-command?   │
  │     ├── ".exit" ───┴──► Thoát vòng lặp, shutdown sạch
  │     ├── ".help" ──────► In hướng dẫn sử dụng
  │     └── khác ─────────► Báo lỗi lệnh không nhận diện
  │
  └──► Chuỗi khác (SQL placeholder)
        │
        ▼
       DatabaseEngine.execute(sql)
        │
        ▼
       ExecutionResult ("Not implemented yet")
        │
        ▼
       CLI in kết quả
```

---

## 2. Checklist công việc cần làm

### Bước 1: Cấu hình `CMakeLists.txt`
- Tiêu chuẩn C++: `C++20`.
- Bật cảnh báo trình biên dịch: `-Wall -Wextra -Wpedantic` (GCC/Clang) hoặc `/W4 /permissive-` (MSVC).
- Khai báo thư mục header: `include/`.
- Tạo target executable `taysql` từ các file nguồn trong `src/`.

### Bước 2: Thiết kế `ExecutionResult`
- Định nghĩa struct kết quả trong namespace `taysql`.
- Các trường cơ bản:
  - `bool is_success`
  - `std::string message`
  - `std::string error_message`

### Bước 3: Thiết kế `DatabaseEngine` Facade
- Khai báo trong header `include/engine/engine.h` (hoặc `database_engine.h`).
- Hiện thực trong `src/engine/engine.cpp`.
- Phương thức tối thiểu:
  - `bool open(const std::string& db_path)`
  - `void close()`
  - `ExecutionResult execute(const std::string& sql)` (trả về thông báo chưa hỗ trợ trong M1).

### Bước 4: Thiết kế `CLI` (REPL Loop)
- Khai báo trong `include/cli/cli.h`, hiện thực trong `src/cli/cli.cpp`.
- Tham chiếu tới `DatabaseEngine`.
- Phương thức `void run()`:
  - In banner chào mừng.
  - Vòng lặp: In prompt `taysql > `, đọc input từ `std::cin` qua `std::getline`.
  - Bỏ qua dòng trống.
  - Xử lý `.exit`: Đặt cờ dừng vòng lặp.
  - Xử lý `.help`: In danh sách lệnh (`.exit`, `.help`, `.tables`, `.schema`).
  - Gửi các câu lệnh còn lại tới `DatabaseEngine::execute()`.

### Bước 5: Điểm khởi chạy `main.cpp`
- Khởi tạo `DatabaseEngine engine;`
- Gọi `engine.open(...)`.
- Khởi tạo `CLI cli(engine);`
- Gọi `cli.run();`
- Dọn dẹp tài nguyên (`engine.close()`) và thoát với return code `0`.

---

## 3. Tiêu chí nghiệm thu (Acceptance Criteria)

1. **Biên dịch:** Chạy `cmake -B build` và `cmake --build build` thành công không có lỗi hay warning nghiêm trọng.
2. **Khởi động:** Chạy file thực thi sinh ra prompt `taysql > `.
3. **Thoát:** Gõ `.exit` thì chương trình thoát an toàn, return code 0.
4. **Trợ giúp:** Gõ `.help` hiển thị danh sách các lệnh meta-commands.
5. **Chuyển tiếp SQL:** Gõ `SELECT * FROM users;` nhận được thông báo rõ ràng từ Engine, chương trình không crash và tiếp tục prompt mới.
6. **Xử lý EOF:** Nhấn `Ctrl+D` (hoặc `Ctrl+Z` trên Windows) thoát êm đẹp, không lặp vô tận.

---

## 4. Mẫu Prompt để hỏi ChatGPT khi bạn tự code Milestone 1

Nếu muốn ChatGPT hỗ trợ sinh code từng file, bạn có thể copy đoạn prompt dưới đây:

```markdown
Tôi đang bắt đầu Milestone 1 (Skeleton) cho dự án C++20 TaySQL.

Mục tiêu Milestone 1:
- Cấu hình CMakeLists.txt chuẩn C++20.
- Xây dựng CLI REPL với prompt "taysql > ", xử lý .exit, .help.
- Tạo DatabaseEngine facade làm điều phối.
- Nhập lệnh không phải meta-command thì DatabaseEngine trả về ExecutionResult thông báo "SQL execution not implemented yet in Milestone 1".
- Main function khởi chạy và thoát sạch sẽ theo RAII.

Hãy viết giúp tôi mã nguồn mẫu chi tiết cho:
1. CMakeLists.txt
2. include/execution/execution_result.h
3. include/engine/engine.h & src/engine/engine.cpp
4. include/cli/cli.h & src/cli/cli.cpp
5. src/main.cpp
```
