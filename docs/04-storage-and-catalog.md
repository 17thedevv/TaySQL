# 04 — Storage, Pager & Catalog

---

## 1. Module Catalog & Table Identity

Catalog quản lý siêu dữ liệu (metadata) của cơ sở dữ liệu:
- Danh sách các bảng, tên bảng.
- Định danh bảng (`TableId`: số nguyên định danh duy nhất, ví dụ `1, 2, ...`).
- Schema từng bảng: danh sách cột, thứ tự, kiểu dữ liệu (`INT`, `TEXT`).

> **Nguyên tắc TableId:** SQL layer sử dụng table name (`"users"`). Storage engine chỉ sử dụng `TableId` (ví dụ `1`). Catalog chịu trách nhiệm mapping giữa `std::string` và `TableId`.

Catalog **không chứa dữ liệu hàng (rows)**.

---

## 2. Row Model & Binary Serialization

Một hàng (Row) là tập hợp các giá trị có kiểu tương ứng với schema:
```cpp
namespace taysql::storage {
    using Value = std::variant<int64_t, std::string>;
    using Row = std::vector<Value>;
}
```
Row không lưu tên cột để tiết kiệm dung lượng. Schema ánh xạ chỉ mục `0 -> id`, `1 -> name`.

### Định dạng nhị phân Explicit (Explicit Binary Layout)
Không serialize raw C++ memory (`memcpy(&row)`). Mỗi row được đóng gói:
- `INT`: Ghi cố định 8 bytes (`int64_t`, little-endian).
- `TEXT`: Ghi 4 bytes độ dài (`uint32_t length`) kèm theo `length` bytes chuỗi ký tự UTF-8.

Ví dụ row `(1, "Alice")`:
`[01 00 00 00 00 00 00 00]` (8 bytes INT) + `[05 00 00 00]` (4 bytes length) + `['A', 'l', 'i', 'c', 'e']` (5 bytes text) = 17 bytes.

---

## 3. Pager Abstraction

Pager là lớp trung gian giữa Storage Engine và File System:
- **Kích thước Page:** Cố định `4096 bytes` (4KB).
- **Trách nhiệm của Pager:**
  - `read_page(PageId page_id, PageBuffer& buffer)`
  - `write_page(PageId page_id, const PageBuffer& buffer)`
  - `allocate_page() -> PageId`
- **Ranh giới:** Pager hoàn toàn không hiểu khái niệm Row, Table, Column hay SQL. Pager chỉ quản lý các block 4096 bytes thô.

---

## 4. Chiến lược Storage: Heap Storage + Full Table Scan

Trong TaySQL v0.1:
- **Không Index, Không B-Tree / B+ Tree.**
- **INSERT:** Luôn ghi nối tiếp (append-only) vào trang hiện tại; nếu trang đầy (không đủ chỗ cho row mới), Pager cấp phát trang 4KB mới.
- **SELECT \*:** Quét toàn bộ các trang từ đầu đến cuối (Full table scan O(n)).
- **Không DELETE / UPDATE** nên chưa cần Free-List trong v0.1.

### Cấu trúc Page v0.1:
```text
Page (4096 bytes)
├── Page Header (16 bytes):
│   ├── uint32_t magic_number (ví dụ 0x54415953 'TAYS')
│   ├── uint32_t row_count
│   ├── uint32_t used_bytes
│   └── uint32_t reserved
└── Records Body:
    ├── record_1_length (uint32_t) + record_1_bytes
    ├── record_2_length (uint32_t) + record_2_bytes
    └── ... (vùng trống còn lại)
```

---

## 5. Cấu trúc lưu trữ trên Disk (Persistence Model)

Để dễ debug và kiểm tra trực quan, v0.1 không gói chung 1 file như SQLite mà lưu trữ dạng thư mục:
```text
mydb.taydb/
├── catalog.bin              # Siêu dữ liệu danh sách bảng, cột, kiểu dữ liệu, TableId
└── tables/
    ├── 1.tbl                # File dữ liệu phân trang 4KB của TableId 1
    ├── 2.tbl                # File dữ liệu phân trang 4KB của TableId 2
    └── ...
```
Sau khi thoát chương trình và mở lại, `catalog.bin` được load lại vào RAM, và khi thực hiện `SELECT *`, dữ liệu trong các file `.tbl` được nạp lên chính xác.
