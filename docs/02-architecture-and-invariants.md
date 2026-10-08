# 02 — Kiến trúc tổng thể & Bất biến (Architecture & Invariants)

---

## 1. Sơ đồ kiến trúc tổng thể

```text
┌──────────────────────┐
│         User         │
└──────────┬───────────┘
           │
           │ SQL / meta-command
           ▼
┌──────────────────────┐
│         CLI          │
└──────────┬───────────┘
           │
           │ SQL text
           ▼
┌──────────────────────┐
│   Database Engine    │  (Facade / Coordinator)
└──────────┬───────────┘
           │
           ▼
┌──────────────────────┐
│        Lexer         │
└──────────┬───────────┘
           │
           │ Token[]
           ▼
┌──────────────────────┐
│        Parser        │
└──────────┬───────────┘
           │
           │ Statement AST
           ▼
┌──────────────────────┐
│       Executor       │
└───────┬────────┬─────┘
        │        │
        ▼        ▼
   ┌─────────┐ ┌─────────┐
   │ Catalog │ │ Storage │
   └─────────┘ └────┬────┘
                    │
                    ▼
               ┌─────────┐
               │  Pager  │
               └────┬────┘
                    │
                    ▼
               ┌─────────┐
               │  Files  │
               └─────────┘
```

---

## 2. Nguyên tắc phụ thuộc (Dependency Rules)

Dependency chỉ được phép **đi một chiều từ trên xuống**:

```text
CLI → Engine → SQL / Executor → Catalog / Storage → Pager → Filesystem
```

Nghiêm cấm các phụ thuộc ngược hoặc ngang vi phạm ranh giới:
- `Pager → Executor`: Pager chỉ biết page 4096 bytes và I/O, không biết khái niệm SQL, Table hay Row.
- `Storage → Parser`: Storage chỉ làm việc với Row nhị phân và TableId, không biết SQL syntax.
- `Parser → CLI` hoặc `Catalog → CLI`: Các tầng dưới hoàn toàn độc lập với giao diện người dùng.

---

## 3. 10 Quy tắc bất biến (Architectural Invariants)

1. **Rule 1:** CLI không parse SQL.
2. **Rule 2:** Parser không truy cập database hoặc filesystem.
3. **Rule 3:** Executor không trực tiếp thao tác filesystem.
4. **Rule 4:** Storage không hiểu cú pháp SQL grammar.
5. **Rule 5:** Pager không hiểu cấu trúc Row hoặc ngữ nghĩa Table.
6. **Rule 6:** Catalog chỉ chứa metadata, không chứa dữ liệu bản ghi (row data).
7. **Rule 7:** `DatabaseEngine` chỉ điều phối luồng, không trở thành god object ôm đồm logic.
8. **Rule 8:** Disk format phải explicit định dạng nhị phân, tuyệt đối không serialize thô con trỏ C++ hay bộ nhớ RAM.
9. **Rule 9:** Quản lý tài nguyên theo nguyên tắc RAII (không rò rỉ file descriptor hay bộ nhớ).
10. **Rule 10:** Không thêm bất kỳ tính năng nào ngoài scope v0.1 trước khi đạt Definition of Done.

---

## 4. Quản lý bộ nhớ & C++ Ownership

- Ưu tiên value semantics và RAII.
- Dùng `std::unique_ptr` khi cần quản lý quyền sở hữu động.
- Dùng tham chiếu (`&` hoặc `const &`) khi truyền đối tượng không chuyển giao ownership.
- Tránh dùng `std::shared_ptr` nếu không thực sự cần chia sẻ quyền sở hữu đa đối tượng.
- Không dùng raw `new` / `delete`.
- Tự động đóng file, giải phóng page buffer trong destructor.

---

## 5. Namespace & Tiêu chuẩn Build

- Toàn bộ mã nguồn thư viện nằm trong namespace `taysql`.
- Các submodule có thể sử dụng nested namespace:
  - `taysql::cli`
  - `taysql::sql`
  - `taysql::execution`
  - `taysql::catalog`
  - `taysql::storage`
- Tiêu chuẩn ngôn ngữ: **C++20**.
- Build system: **CMake** (`-Wall -Wextra` hoặc `/W4`).
- Không thêm thư viện bên ngoài (external dependencies) nếu Thư viện chuẩn C++ (STL) đã đáp ứng đủ.
