# 01 — Phạm vi dự án & Tính năng (Scope & Features)

**Project:** TaySQL  
**Version:** 0.1  
**Language:** C++20  
**Primary Goal:** Học kiến trúc hệ quản trị cơ sở dữ liệu và C++ systems programming bằng cách xây một database chạy end-to-end thực sự.

---

## 1. Mục tiêu v0.1

TaySQL v0.1 không cố gắng trở thành SQLite thu nhỏ đầy đủ. Mục tiêu là xây dựng một pipeline hoàn chỉnh:

```text
SQL → Lexer → Parser → AST → Executor → Catalog + Storage → Pager → Disk
```

Sau v0.1, TaySQL phải có khả năng:
1. Khởi động bằng CLI.
2. Nhận tập con nhỏ của SQL.
3. Parse SQL thành cấu trúc dữ liệu có kiểu.
4. Tạo bảng (`CREATE TABLE`).
5. Thêm bản ghi (`INSERT`).
6. Đọc bản ghi (`SELECT *`).
7. Lưu database xuống disk.
8. Tắt chương trình, khởi động lại, và đọc lại chính xác dữ liệu đã lưu.

---

## 2. SQL được hỗ trợ trong v0.1

### 2.1 CREATE TABLE
```sql
CREATE TABLE users (
    id INT,
    name TEXT
);
```
- Hỗ trợ: `CREATE TABLE <table_name> (<column_name> <type>, ...);`
- Kiểu dữ liệu hỗ trợ: `INT`, `TEXT`.
- Chưa hỗ trợ: `PRIMARY KEY`, `NOT NULL`, `UNIQUE`, `DEFAULT`, `FOREIGN KEY`, `CHECK`, `AUTO_INCREMENT`.

### 2.2 INSERT
```sql
INSERT INTO users VALUES (1, 'Alice');
```
- Chỉ hỗ trợ cú pháp đầy đủ theo schema: `INSERT INTO <table> VALUES (...);`
- Số lượng và kiểu giá trị phải khớp với schema.
- Chưa hỗ trợ: Chỉ định danh sách cột như `INSERT INTO users (id, name) VALUES (...)`.

### 2.3 SELECT
```sql
SELECT * FROM users;
```
- Bắt buộc hỗ trợ: `SELECT * FROM <table_name>;`
- Mở rộng (nếu kịp): `SELECT id, name FROM users;`
- Chưa hỗ trợ: `WHERE`, `JOIN`, `GROUP BY`, `ORDER BY`, `LIMIT`, `DISTINCT`, aggregate functions, subquery.

---

## 3. CLI Meta-Commands

Các lệnh điều khiển môi trường REPL, do CLI tự xử lý (không chuyển xuống SQL parser):
- `.exit`: Thoát chương trình TaySQL.
- `.help`: Hiển thị danh sách các lệnh được hỗ trợ.
- `.tables`: Liệt kê các bảng hiện có trong database.
- `.schema <table>`: Hiển thị định nghĩa các cột và kiểu dữ liệu của bảng.

---

## 4. Scope Freeze — Những thứ CỐ Ý KHÔNG LÀM trong v0.1

Để đảm bảo hoàn thành dự án, các tính năng sau được **đóng băng (freeze)**, tuyệt đối không thêm trước khi v0.1 hoàn thành:
- Không `UPDATE`, `DELETE`, `WHERE`, `JOIN`, `INDEX`, `B-Tree` / `B+ Tree`.
- Không query optimizer, transactions, ACID, WAL, MVCC.
- Không đa luồng (threads), async, multiple clients, TCP server, mạng.
- Không tương thích định dạng file của SQLite.
- Không hỗ trợ các kiểu: `NULL`, `FLOAT`, `BOOLEAN`, `DATE`.

---

## 5. Tuyên bố Scope

> **TaySQL 0.1 là một persistent single-process SQL database engine nhỏ, viết bằng C++20, hỗ trợ CREATE TABLE, INSERT và SELECT trên INT/TEXT, sử dụng handwritten lexer/parser, typed AST, executor, persistent catalog, heap table storage và page-based disk I/O.**
