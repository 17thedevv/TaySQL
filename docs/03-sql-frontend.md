# 03 — SQL Frontend: Lexer, Parser & AST

---

## 1. Module CLI (Command-Line Interface)

- **Trách nhiệm:**
  - Nhận input từ terminal (`std::getline`).
  - Vòng lặp REPL (Read-Eval-Print Loop) với prompt `taysql > `.
  - Nhận diện và thực thi meta-command (`.exit`, `.help`, `.tables`, `.schema`).
  - Chuyển tiếp câu lệnh SQL sang `DatabaseEngine::execute(sql)`.
  - Hiển thị kết quả trả về (`ExecutionResult`) ra màn hình terminal.
- **Ranh giới:** CLI không parse SQL, không đọc file `.tbl`, không chạm vào Catalog/Storage.

---

## 2. Module DatabaseEngine (Facade Coordinator)

`DatabaseEngine` đóng vai trò coordinator cho toàn hệ thống:
```cpp
namespace taysql {
class DatabaseEngine {
public:
    Result<void> open(const std::string& db_path);
    ExecutionResult execute(const std::string& sql);
    void close();
};
}
```
Engine sở hữu hoặc điều phối:
- `Catalog`
- `StorageEngine`
- `Executor`

---

## 3. Module Lexer & Token Model

Lexer phân tích chuỗi SQL thành mảng các `Token`. Lexer không hiểu ngữ nghĩa schema, chỉ xử lý từ vựng.

### Các Token cần cho v0.1:
- **Keywords:** `CREATE`, `TABLE`, `INSERT`, `INTO`, `VALUES`, `SELECT`, `FROM`
- **Data Types:** `INT`, `TEXT`
- **Literals & Identifiers:** `IDENTIFIER` (tên bảng/cột), `INTEGER_LITERAL`, `STRING_LITERAL`
- **Punctuation:** `STAR` (`*`), `LEFT_PAREN` (`(`), `RIGHT_PAREN` (`)`), `COMMA` (`,`), `SEMICOLON` (`;`)
- **Control:** `EOF_TOKEN`, `INVALID`

Ví dụ:
```sql
INSERT INTO users VALUES (1, 'Alice');
```
Được tách thành:
`INSERT`, `INTO`, `IDENTIFIER("users")`, `VALUES`, `LEFT_PAREN`, `INTEGER_LITERAL(1)`, `COMMA`, `STRING_LITERAL("Alice")`, `RIGHT_PAREN`, `SEMICOLON`, `EOF_TOKEN`.

---

## 4. Module Parser & AST Model

Parser nhận `std::vector<Token>` và sinh ra một `Statement` có kiểu (Typed AST). Parser chỉ xác thực cú pháp (syntax grammar), chưa kiểm tra bảng/cột có tồn tại hay không.

### 4.1 Cấu trúc AST với `std::variant`
Thay vì kế thừa phân cấp phức tạp, v0.1 sử dụng data-oriented representation:

```cpp
namespace taysql::sql {

enum class DataType {
    INT,
    TEXT
};

struct ColumnDefinition {
    std::string name;
    DataType type;
};

struct CreateTableStatement {
    std::string table_name;
    std::vector<ColumnDefinition> columns;
};

using LiteralValue = std::variant<int64_t, std::string>;

struct InsertStatement {
    std::string table_name;
    std::vector<LiteralValue> values;
};

struct SelectStatement {
    std::string table_name;
    bool select_all = true;
    std::vector<std::string> selected_columns; // Mở rộng
};

using Statement = std::variant<
    CreateTableStatement,
    InsertStatement,
    SelectStatement
>;

}
```
