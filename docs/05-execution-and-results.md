# 05 — Execution, Error & REPL Lifecycle

---

## 1. Module Executor & Semantic Validation

Executor nhận Typed AST từ Parser và thực thi hành động:

### Luồng thực thi:
1. **`CreateTableStatement`:**
   - Kiểm tra trùng lặp bảng trong `Catalog`.
   - Đăng ký bảng mới vào `Catalog` (cấp phát `TableId`).
   - Gọi `StorageEngine::create_table(table_id)`.
2. **`InsertStatement`:**
   - Tìm kiếm schema bảng trong `Catalog`.
   - Kiểm tra ngữ nghĩa: số lượng cột và kiểu dữ liệu có khớp với schema không.
   - Chuyển đổi thành `storage::Row` và gọi `StorageEngine::insert(table_id, row)`.
3. **`SelectStatement`:**
   - Kiểm tra sự tồn tại của bảng.
   - Gọi `StorageEngine::scan(table_id)` để lấy toàn bộ các dòng.
   - Đóng gói thành `ExecutionResult` chứa metadata cột và mảng dữ liệu.

---

## 2. Mô hình Xử lý Lỗi (Error Model) & Result<T>

- Tuyệt đối không `std::cout` lỗi từ sâu trong engine.
- Không dùng con trỏ `nullptr`, `-1` hay cờ `bool` đơn giản để báo lỗi chung chung.
- Định nghĩa kiểu dữ liệu lỗi rõ ràng:

```cpp
namespace taysql::common {

enum class ErrorCode {
    LEX_ERROR,
    PARSE_ERROR,
    TABLE_NOT_FOUND,
    TABLE_ALREADY_EXISTS,
    TYPE_MISMATCH,
    COLUMN_COUNT_MISMATCH,
    STORAGE_ERROR,
    CORRUPTED_FILE,
    IO_ERROR
};

struct Error {
    ErrorCode code;
    std::string message;
};

template <typename T>
using Result = std::variant<T, Error>;

}
```

---

## 3. ExecutionResult & CLI Rendering

`DatabaseEngine::execute()` không tự in màn hình mà trả về cấu trúc kết quả:

```cpp
namespace taysql::execution {

struct ExecutionResult {
    bool is_success = true;
    std::string message;                       // Ví dụ: "Table created." hoặc "1 row inserted."
    std::vector<std::string> column_names;    // Dùng cho SELECT
    std::vector<std::vector<std::string>> rows;// Dùng cho SELECT (đã format string)
    std::string error_message;                 // Thông báo khi lỗi
};

}
```
CLI là nơi duy nhất chịu trách nhiệm vẽ bảng hoặc in thông báo kết quả ra terminal.

---

## 4. Vòng đời REPL (REPL Lifecycle)

1. **Khởi động (Startup):**
   - Khởi tạo `DatabaseEngine`.
   - Nạp `catalog.bin` (nếu tồn tại) hoặc chuẩn bị database mới.
   - Chạy giao diện CLI REPL.
2. **Vòng lặp tương tác (Interaction Loop):**
   - In `taysql > `, chờ người dùng nhập lệnh.
   - Nếu là lệnh meta (`.exit`, `.help`, `.tables`, `.schema`): CLI xử lý trực tiếp.
   - Nếu là lệnh SQL: CLI gửi chuỗi sang `engine.execute(sql)` -> nhận `ExecutionResult` -> render ra console.
3. **Thoát an toàn (Shutdown):**
   - Người dùng gõ `.exit` hoặc gửi tín hiệu ngắt (EOF/Ctrl+C/Ctrl+D).
   - Flush toàn bộ các trang bẩn (dirty pages) xuống đĩa.
   - Đóng file handles.
   - Huỷ `DatabaseEngine` theo cơ chế RAII -> Thoát chương trình sạch sẽ (exit 0).
