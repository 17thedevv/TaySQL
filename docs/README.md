# TaySQL v0.1 — Documentation Index

Tài liệu thiết kế kiến trúc hệ thống của **TaySQL v0.1** (C++20 Embedded Mini SQL Database Engine) được chia thành các chuyên đề chuyên biệt dưới đây để dễ đọc hiểu, tra cứu và sử dụng làm context cho AI:

---

## Danh mục tài liệu thiết kế

1. [01. Phạm vi dự án & Tính năng (Scope & Features)](file:///d:/Project-17/LearnCpp/TaySQL/docs/01-scope-and-features.md)
   * Mục tiêu v0.1, cú pháp SQL được hỗ trợ (`CREATE TABLE`, `INSERT`, `SELECT *`).
   * Meta-commands của CLI (`.exit`, `.help`, `.tables`, `.schema`).
   * Scope freeze (những tính năng cố ý chưa làm) và lộ trình tương lai (v0.2+).

2. [02. Kiến trúc tổng thể & Bất biến (Architecture & Invariants)](file:///d:/Project-17/LearnCpp/TaySQL/docs/02-architecture-and-invariants.md)
   * Sơ đồ pipeline toàn diện (CLI -> Engine -> SQL -> Catalog/Storage -> Pager -> Disk).
   * Nguyên tắc phụ thuộc 1 chiều (Dependency rules).
   * 10 nguyên tắc bất biến (Architectural Invariants).
   * Quy tắc quản lý bộ nhớ, C++ ownership, namespace `taysql`, tiêu chuẩn build C++20.

3. [03. SQL Frontend: Lexer, Parser & AST](file:///d:/Project-17/LearnCpp/TaySQL/docs/03-sql-frontend.md)
   * Token model & Lexer grammar.
   * Parser & Typed AST (`std::variant` cho Statements: CreateTable, Insert, Select).
   * Chi tiết các cấu trúc dữ liệu AST.

4. [04. Storage, Pager & Catalog](file:///d:/Project-17/LearnCpp/TaySQL/docs/04-storage-and-catalog.md)
   * Catalog metadata & `TableId` abstraction.
   * Row Model & Định dạng nhị phân explicit serialization.
   * Pager (4096 bytes page) & Chiến lược Heap Storage (append-only).
   * Định dạng lưu trữ trên disk: `catalog.bin` & `tables/<id>.tbl`.

5. [05. Execution, Error & REPL Lifecycle](file:///d:/Project-17/LearnCpp/TaySQL/docs/05-execution-and-results.md)
   * Executor & Semantic validation.
   * Mô hình `Error` & `Result<T>` không ném exception bừa bãi.
   * `ExecutionResult` và CLI rendering.
   * Vòng đời REPL (Startup -> Loop -> RAII Shutdown).

6. [06. Lộ trình phát triển & Milestones](file:///d:/Project-17/LearnCpp/TaySQL/docs/06-roadmap-and-milestones.md)
   * Nguyên tắc Vertical Slices (không làm hoàn chỉnh theo lớp).
   * Danh sách 9 Milestones từ Milestone 1 (Skeleton) đến Milestone 9 (Integration Freeze).
   * Chiến lược kiểm thử (Unit & Integration tests) và Definition of Done cho v0.1.

---

## Kế hoạch triển khai chi tiết từng Milestone

* [Milestone 1 — Skeleton (Khung dự án & REPL CLI)](file:///d:/Project-17/LearnCpp/TaySQL/docs/milestone-01-skeleton.md)
  * Thiết kế luồng tương tác, checklist từng file, tiêu chí nghiệm thu và mẫu prompt ChatGPT cho Milestone 1.
