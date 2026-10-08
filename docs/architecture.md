# TaySQL v0.1 — System Design Specification

**Project:** TaySQL  
**Version:** 0.1  
**Language:** C++20  
**Type:** Embedded / CLI mini SQL database engine  
**Primary goal:** Học kiến trúc hệ quản trị cơ sở dữ liệu và C++ systems programming bằng cách xây một database chạy end-to-end thực sự.

---

# 1. Mục tiêu của TaySQL v0.1

TaySQL v0.1 không cố gắng trở thành SQLite thu nhỏ đầy đủ.

Mục tiêu là xây được một hệ thống hoàn chỉnh có luồng:

```text
SQL
 ↓
Lexer
 ↓
Parser
 ↓
AST
 ↓
Executor
 ↓
Catalog + Storage
 ↓
Pager
 ↓
Disk
```

Sau v0.1, TaySQL phải có khả năng:

1. Khởi động bằng CLI.
2. Nhận một tập con nhỏ của SQL.
3. Parse SQL thành cấu trúc dữ liệu có kiểu.
4. Tạo bảng.
5. Insert row.
6. Select row.
7. Lưu database xuống disk.
8. Tắt chương trình.
9. Khởi động lại.
10. Đọc lại chính dữ liệu đã lưu.

Nếu toàn bộ pipeline trên hoạt động ổn định thì v0.1 được xem là hoàn thành.

---

# 2. Product Scope v0.1

## 2.1 SQL được hỗ trợ

### CREATE TABLE

```sql
CREATE TABLE users (
    id INT,
    name TEXT
);
```

v0.1 hỗ trợ:

```text
CREATE TABLE <table_name> (
    <column_name> <type>,
    ...
);
```

Data type:

```text
INT
TEXT
```

Chưa hỗ trợ:

```text
PRIMARY KEY
NOT NULL
UNIQUE
DEFAULT
FOREIGN KEY
CHECK
AUTO_INCREMENT
```

---

## 2.2 INSERT

```sql
INSERT INTO users VALUES (1, 'Alice');
```

Chỉ hỗ trợ:

```text
INSERT INTO <table> VALUES (...);
```

Số lượng và type của value phải khớp schema.

Chưa hỗ trợ:

```sql
INSERT INTO users (id, name) VALUES (...);
```

---

## 2.3 SELECT

v0.1 hỗ trợ:

```sql
SELECT * FROM users;
```

Có thể mở rộng trong cuối v0.1 nếu implementation đã ổn:

```sql
SELECT id, name FROM users;
```

Nhưng `SELECT *` là yêu cầu bắt buộc.

Không có:

```text
WHERE
JOIN
GROUP BY
ORDER BY
LIMIT
DISTINCT
aggregate functions
subquery
```

---

# 3. CLI meta-command

CLI hỗ trợ một số command không phải SQL:

```text
.exit
.help
.tables
.schema <table>
```

Ý nghĩa:

```text
.exit
    thoát TaySQL

.help
    hiển thị command được hỗ trợ

.tables
    liệt kê table

.schema users
    hiển thị schema của users
```

Meta-command được CLI xử lý.

SQL không được CLI parse.

---

# 4. Những thứ cố ý KHÔNG làm trong v0.1

Đây là phần scope freeze.

Không thêm các tính năng sau trước khi v0.1 hoàn thành:

```text
UPDATE
DELETE
WHERE
JOIN
INDEX
B-Tree
B+ Tree
query optimizer
transactions
ACID
WAL
MVCC
concurrency
multiple clients
TCP server
authentication
foreign key
constraints
ALTER TABLE
DROP TABLE
NULL
FLOAT
BOOLEAN
DATE
prepared statement
stored procedure
network protocol
```

Không có thread.

Không có async.

Không có distributed database.

Không có ORM.

Không cố tương thích file format SQLite.

Nếu một tính năng không nằm trong scope trên, mặc định chuyển sang v0.2+.

---

# 5. Kiến trúc tổng thể

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
│   Database Engine    │
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

# 6. Nguyên tắc dependency

Dependency chỉ được đi xuống.

```text
CLI
 ↓
Engine
 ↓
SQL / Executor
 ↓
Catalog / Storage
 ↓
Pager
```

Không được có:

```text
Pager → Executor
Storage → Parser
Parser → CLI
Catalog → CLI
```

Ví dụ Pager hoàn toàn không biết:

```text
SELECT
INSERT
users
column
SQL
```

Pager chỉ biết:

```text
page id
read page
write page
```

---

# 7. Module: CLI

## Responsibility

CLI chịu trách nhiệm:

```text
đọc input
in output
REPL loop
meta-command
forward SQL tới DatabaseEngine
```

CLI không được:

```text
parse SQL
access table file
serialize row
đọc page
thay đổi catalog trực tiếp
```

Luồng:

```text
taydb> SELECT * FROM users;

CLI
 ↓
DatabaseEngine::execute(...)
 ↓
ExecutionResult
 ↓
CLI render result
```

---

# 8. Module: DatabaseEngine

DatabaseEngine là facade/coordinator của hệ thống.

Nó không nên chứa toàn bộ implementation database.

Vai trò:

```text
receive SQL
    ↓
lexer
    ↓
parser
    ↓
executor
    ↓
return result
```

Có thể hình dung API:

```text
DatabaseEngine
    open(database)
    execute(sql)
    close()
```

Engine sở hữu hoặc điều phối:

```text
Catalog
StorageEngine
Executor
```

---

# 9. Module: Lexer

Lexer biến:

```sql
INSERT INTO users VALUES (1, 'Alice');
```

thành:

```text
INSERT
INTO
IDENTIFIER(users)
VALUES
LEFT_PAREN
INTEGER(1)
COMMA
STRING("Alice")
RIGHT_PAREN
SEMICOLON
EOF
```

Lexer không hiểu schema.

Lexer cũng không biết table `users` có tồn tại hay không.

Nó chỉ xử lý lexical syntax.

---

# 10. Token model

Các token cần cho v0.1:

```text
CREATE
TABLE
INSERT
INTO
VALUES
SELECT
FROM

INT
TEXT

IDENTIFIER
INTEGER_LITERAL
STRING_LITERAL

STAR

LEFT_PAREN
RIGHT_PAREN
COMMA
SEMICOLON

EOF
```

Có thể bổ sung token lỗi:

```text
INVALID
```

hoặc trả diagnostic trực tiếp.

---

# 11. Module: Parser

Parser nhận:

```text
vector<Token>
```

và tạo:

```text
Statement
```

Parser chỉ kiểm tra syntax.

Ví dụ:

```sql
SELECT * FROM users;
```

trở thành:

```text
SelectStatement
    table = "users"
    projection = All
```

Parser chưa kiểm tra `users` có tồn tại.

Việc đó thuộc semantic/execution layer.

---

# 12. AST / Statement Model

Top-level statement:

```text
Statement
 ├── CreateTableStatement
 ├── InsertStatement
 └── SelectStatement
```

Trong C++, v0.1 nên ưu tiên data-oriented representation thay vì hierarchy phức tạp.

Ví dụ về mặt thiết kế:

```text
Statement =
    variant<
        CreateTableStatement,
        InsertStatement,
        SelectStatement
    >
```

---

# 13. CreateTableStatement

Conceptual representation:

```text
CreateTableStatement
    table_name

    columns:
        ColumnDefinition
            name
            type
```

Ví dụ:

```sql
CREATE TABLE users (
    id INT,
    name TEXT
);
```

AST:

```text
CreateTableStatement
│
├── table_name = users
│
└── columns
    ├── id   : INT
    └── name : TEXT
```

---

# 14. InsertStatement

Ví dụ:

```sql
INSERT INTO users VALUES (1, 'Alice');
```

AST:

```text
InsertStatement
│
├── table_name = users
│
└── values
    ├── Integer(1)
    └── String("Alice")
```

Value representation:

```text
Value
    Integer
    String
```

Conceptually:

```text
variant<int64, string>
```

---

# 15. SelectStatement

v0.1 bắt buộc:

```sql
SELECT * FROM users;
```

AST:

```text
SelectStatement
│
├── table_name = users
└── projection = All
```

Nếu selected columns được thêm:

```sql
SELECT id, name FROM users;
```

thì:

```text
projection = ["id", "name"]
```

---

# 16. Module: Catalog

Catalog chứa metadata.

Catalog không chứa row data.

Ví dụ:

```text
users
│
├── table_id = 1
│
├── id   : INT
└── name : TEXT
```

Catalog quản lý:

```text
table name
table id
schema
columns
types
storage location
```

Các operation cơ bản:

```text
createTable
findTable
listTables
getSchema
```

---

# 17. Table identity

Không dùng table name trực tiếp làm mọi thứ trong storage.

Mỗi table được cấp:

```text
TableId
```

Ví dụ:

```text
users  → TableId 1
posts  → TableId 2
```

Storage sử dụng `TableId`.

SQL layer sử dụng table name.

Catalog mapping:

```text
"users"
    ↓
TableId(1)
```

Điều này giúp separation tốt hơn.

---

# 18. Module: Executor

Executor biến AST thành hành động thật.

Ví dụ:

```text
CreateTableStatement
        ↓
Executor
        ↓
Catalog::createTable()
        ↓
Storage::createTable()
```

INSERT:

```text
InsertStatement
        ↓
lookup schema
        ↓
validate values
        ↓
construct Row
        ↓
Storage::insert()
```

SELECT:

```text
SelectStatement
        ↓
lookup table
        ↓
Storage::scan()
        ↓
Rows
        ↓
ExecutionResult
```

Executor hiểu cả AST và database concepts.

Đây là boundary giữa SQL frontend và database backend.

---

# 19. Semantic validation

Parser chỉ kiểm tra:

```text
SQL có đúng grammar không?
```

Executor hoặc semantic layer kiểm tra:

```text
table có tồn tại không?
column count có đúng không?
type có khớp không?
table có bị duplicate không?
```

Ví dụ:

```sql
INSERT INTO users VALUES ('hello', 123);
```

Nếu schema:

```text
id INT
name TEXT
```

thì phải reject.

---

# 20. Row model

Một row là tập value theo đúng schema.

Ví dụ:

```text
users:
    id INT
    name TEXT
```

Row:

```text
[
    Integer(1),
    String("Alice")
]
```

Row không cần lưu lại column name.

Schema đã biết:

```text
index 0 → id
index 1 → name
```

Điều này giảm duplication.

---

# 21. Storage Engine

Storage Engine chịu trách nhiệm:

```text
create table storage
insert row
scan rows
serialize row
deserialize row
```

Storage không parse SQL.

API conceptual:

```text
createTable(TableId)
insert(TableId, Row)
scan(TableId) -> Rows
```

---

# 22. Storage strategy v0.1

TaySQL v0.1 sử dụng:

> Heap storage + full table scan.

Không index.

Không B-tree.

INSERT luôn append.

SELECT đọc toàn bộ row của table.

Complexity:

```text
INSERT ≈ O(1) amortized
SELECT * = O(n)
```

Điều này cố ý đơn giản.

Index thuộc phiên bản sau.

---

# 23. Pager

Pager là abstraction giữa Storage Engine và filesystem.

Page size v0.1:

```text
4096 bytes
```

Pager biết:

```text
PageId
readPage(PageId)
writePage(PageId)
allocatePage()
```

Pager không biết:

```text
Row
Table
SQL
INT
TEXT
```

---

# 24. Page model v0.1

Do v0.1:

```text
không DELETE
không UPDATE
```

nên page format có thể rất đơn giản.

Concept:

```text
Page
│
├── Header
│   ├── magic/version
│   ├── row_count
│   └── used_bytes
│
└── Records
    ├── record_length
    ├── record bytes
    ├── record_length
    └── record bytes
```

INSERT:

```text
serialize Row
      ↓
record bytes
      ↓
nếu page hiện tại đủ chỗ
    append
else
    allocate page mới
```

Không cần free-list trong v0.1.

---

# 25. Row serialization

Ví dụ row:

```text
1, "Alice"
```

Conceptual binary:

```text
INT:
    fixed 8 bytes

TEXT:
    length
    bytes
```

Ví dụ:

```text
[1][5]["Alice"]
```

Format thực tế phải được document rõ ràng trước khi implementation.

Không được serialize bằng cách dump raw C++ object memory.

Không được ghi:

```text
std::string internal representation
pointer
vtable
```

xuống disk.

Chỉ ghi explicit binary format.

---

# 26. Persistence model

v0.1 không bắt buộc phải dùng một single file như SQLite.

TaySQL chỉ SQLite-like về:

```text
embedded usage
SQL interface
local persistence
```

Không tương thích SQLite file format.

Database trên disk có thể có dạng:

```text
mydb.taydb/
│
├── catalog.bin
│
└── tables/
    ├── 1.tbl
    ├── 2.tbl
    └── ...
```

Ví dụ:

```text
users → TableId 1 → tables/1.tbl
```

Thiết kế này dễ debug và phù hợp v0.1.

Single-file database có thể trở thành mục tiêu v0.2/v0.3.

---

# 27. Catalog persistence

Catalog phải persistent.

Nếu chạy:

```sql
CREATE TABLE users (
    id INT,
    name TEXT
);
```

rồi tắt chương trình, khi mở lại:

```text
.tables
```

vẫn phải thấy:

```text
users
```

`catalog.bin` lưu:

```text
TableId
table name
column count
column names
column types
```

---

# 28. Error model

Không sử dụng:

```text
cout từ sâu trong engine
```

để báo lỗi.

Không dùng:

```text
nullptr
false
-1
```

cho mọi loại lỗi.

Engine cần representation có kiểu:

```text
Error
│
├── LexError
├── ParseError
├── TableNotFound
├── TableAlreadyExists
├── TypeMismatch
├── ColumnCountMismatch
├── StorageError
├── CorruptedFile
└── IOError
```

Mỗi error có:

```text
kind
message
```

Lexer/parser có thể bổ sung:

```text
source position
```

---

# 29. Result model

Các module trả:

```text
Result<T>
```

conceptually:

```text
success(T)

hoặc

error(Error)
```

C++20 không có `std::expected`, nên TaySQL có thể tự xây một abstraction nhỏ hoặc sử dụng `std::variant`.

Không cần tạo một error framework quá phức tạp trong v0.1.

---

# 30. ExecutionResult

`DatabaseEngine::execute()` không nên trực tiếp print.

Nó trả về một result object.

Ví dụ:

```text
ExecutionResult
│
├── Success
├── Message
├── Rows
└── Error
```

Ví dụ CREATE:

```text
Table created.
```

INSERT:

```text
1 row inserted.
```

SELECT:

```text
columns
rows
```

CLI chịu trách nhiệm render.

Sau này một GUI/network server có thể dùng cùng DatabaseEngine.

---

# 31. REPL lifecycle

Startup:

```text
main
 ↓
DatabaseEngine open
 ↓
Catalog load
 ↓
CLI run
```

REPL:

```text
while running
    ↓
read input
    ↓
meta-command?
 ┌─────┴─────┐
 yes         no
 ↓            ↓
CLI         Engine.execute(sql)
              ↓
            result
              ↓
            render
```

Shutdown:

```text
.exit
 ↓
flush pending pages
 ↓
close files
 ↓
destroy engine
 ↓
exit
```

RAII phải được tận dụng để cleanup resource.

---

# 32. C++ ownership rules

v0.1 nên có ownership rõ ràng.

Ưu tiên:

```text
object ownership
RAII
value semantics
unique_ptr khi cần dynamic ownership
reference khi object không sở hữu
```

Không dùng `shared_ptr` nếu không thực sự cần shared ownership.

Không sử dụng raw `new/delete` trong application code trừ khi có lý do cụ thể.

File handles và resources phải cleanup bằng destructor.

---

# 33. Proposed source tree

```text
TaySQL/
│
├── CMakeLists.txt
│
├── README.md
│
├── docs/
│   └── architecture.md
│
├── include/
│   └── taysql/
│       ├── cli/
│       │   └── cli.h
│       │
│       ├── engine/
│       │   └── database_engine.h
│       │
│       ├── sql/
│       │   ├── token.h
│       │   ├── lexer.h
│       │   └── ast.h
│       │   └── parser.h
│       │
│       ├── execution/
│       │   ├── executor.h
│       │   └── execution_result.h
│       │
│       ├── catalog/
│       │   ├── catalog.h
│       │   └── schema.h
│       │
│       ├── storage/
│       │   ├── storage_engine.h
│       │   ├── row.h
│       │   ├── serializer.h
│       │   └── pager.h
│       │
│       └── common/
│           ├── error.h
│           ├── result.h
│           └── types.h
│
├── src/
│   ├── main.cpp
│   │
│   ├── cli/
│   │   └── cli.cpp
│   │
│   ├── engine/
│   │   └── database_engine.cpp
│   │
│   ├── sql/
│   │   ├── lexer.cpp
│   │   └── parser.cpp
│   │
│   ├── execution/
│   │   └── executor.cpp
│   │
│   ├── catalog/
│   │   └── catalog.cpp
│   │
│   └── storage/
│       ├── storage_engine.cpp
│       ├── serializer.cpp
│       └── pager.cpp
│
└── tests/
    ├── lexer_tests.cpp
    ├── parser_tests.cpp
    ├── catalog_tests.cpp
    ├── storage_tests.cpp
    └── integration_tests.cpp
```

Không cần tạo tất cả file ngay lập tức.

Đây là target architecture.

File chỉ nên được tạo khi module bắt đầu được implement.

---

# 34. Namespace

Toàn bộ library code đặt trong:

```text
taysql
```

Các module có thể dùng nested namespace:

```text
taysql::sql
taysql::storage
taysql::catalog
```

Tránh global namespace pollution.

---

# 35. Build requirements

v0.1:

```text
C++20
CMake
compiler warnings enabled
Debug + Release build
```

Core engine ưu tiên standard library.

Không thêm dependency nếu standard library đã giải quyết được vấn đề.

---

# 36. Testing strategy

## Unit tests

### Lexer

```text
keyword
identifier
integer
string
punctuation
invalid token
```

### Parser

```text
CREATE TABLE
INSERT
SELECT
invalid syntax
```

### Catalog

```text
create table
duplicate table
lookup
persistence
```

### Storage

```text
serialize row
deserialize row
insert
scan
multiple pages
```

---

# 37. Integration tests

Test quan trọng nhất v0.1:

```text
open database
↓
CREATE TABLE users (...)
↓
INSERT INTO users VALUES (...)
↓
SELECT * FROM users
↓
verify result
↓
close database
↓
open database lại
↓
SELECT * FROM users
↓
verify dữ liệu vẫn tồn tại
```

Nếu test này pass, phần lớn architecture chính đã kết nối được với nhau.

---

# 38. Không mock quá mức

Storage test nên sử dụng temporary file thật.

Database engine integration test nên chạy real pipeline:

```text
Lexer
Parser
Executor
Catalog
Storage
Pager
Disk
```

Không chỉ test từng module cô lập.

---

# 39. Implementation order

Không implement project theo từng layer hoàn chỉnh.

Không làm:

```text
viết toàn bộ lexer
↓
toàn bộ parser
↓
toàn bộ storage
↓
sau vài tuần mới chạy thử
```

Thay vào đó sử dụng vertical slices.

---

# 40. Milestone 1 — Skeleton

Mục tiêu:

```text
build được
CLI chạy
.exit hoạt động
```

Architecture:

```text
main
 ↓
CLI
 ↓
DatabaseEngine
```

Chưa SQL.

---

# 41. Milestone 2 — SELECT parsing

Support:

```sql
SELECT * FROM users;
```

Pipeline:

```text
text
 ↓
lexer
 ↓
tokens
 ↓
parser
 ↓
SelectStatement
```

Chưa cần database thực.

Mục tiêu là frontend hoạt động.

---

# 42. Milestone 3 — In-memory Catalog

Support:

```sql
CREATE TABLE users (
    id INT,
    name TEXT
);
```

Pipeline:

```text
CREATE
 ↓
Parser
 ↓
CreateTableStatement
 ↓
Executor
 ↓
Catalog
```

Catalog lúc này có thể nằm RAM.

---

# 43. Milestone 4 — In-memory rows

Support:

```sql
INSERT INTO users VALUES (1, 'Alice');
SELECT * FROM users;
```

Storage tạm thời có thể sử dụng container STL trong RAM.

Đây là intermediate implementation, không phải final architecture.

Mục tiêu:

```text
SQL pipeline hoàn chỉnh trước
```

---

# 44. Milestone 5 — Row serialization

Thay row RAM thuần túy bằng serialization format.

Test:

```text
Row
 ↓
bytes
 ↓
Row
```

phải round-trip chính xác.

---

# 45. Milestone 6 — Pager

Implement:

```text
4096-byte page
read page
write page
allocate page
```

Pager được test độc lập.

---

# 46. Milestone 7 — Persistent table storage

Thay in-memory rows bằng:

```text
Storage Engine
 ↓
Pager
 ↓
.tbl file
```

INSERT phải persistent.

SELECT phải scan page.

---

# 47. Milestone 8 — Persistent Catalog

`catalog.bin` được load/save.

Sau restart:

```text
schema vẫn tồn tại
table data vẫn tồn tại
```

---

# 48. Milestone 9 — Integration freeze

Test end-to-end:

```sql
CREATE TABLE users (
    id INT,
    name TEXT
);

INSERT INTO users VALUES (1, 'Alice');
INSERT INTO users VALUES (2, 'Bob');

SELECT * FROM users;
```

Output tương đương:

```text
id | name
---+------
1  | Alice
2  | Bob
```

Restart TaySQL.

Chạy lại:

```sql
SELECT * FROM users;
```

vẫn phải có hai row.

---

# 49. Definition of Done — TaySQL 0.1

v0.1 chỉ được coi là hoàn thành khi toàn bộ điều kiện sau đúng:

### Build

- Project build bằng CMake.
- C++20.
- Không warning nghiêm trọng trong compiler configuration chính.

### CLI

- REPL hoạt động.
- `.exit`
- `.help`
- `.tables`
- `.schema`

### SQL

- `CREATE TABLE`
- `INSERT INTO ... VALUES`
- `SELECT * FROM`

### Types

- `INT`
- `TEXT`

### Parser

- Lexer riêng.
- Parser riêng.
- Typed AST.
- Syntax error không crash.

### Catalog

- Table schema.
- Table lookup.
- Duplicate table diagnostic.
- Persistent catalog.

### Storage

- Row serialization.
- Page abstraction.
- Disk persistence.
- Table scan.

### Reliability

- Invalid SQL không crash.
- Table không tồn tại không crash.
- Wrong value type không crash.
- Corrupt/truncated storage được detect hoặc fail safely.

### Persistence

Sau:

```text
create
insert
exit
restart
select
```

dữ liệu vẫn chính xác.

### Tests

Có unit tests cho:

```text
lexer
parser
catalog
serialization
pager
storage
```

và ít nhất một integration test end-to-end.

---

# 50. Architectural invariants

Các quy tắc sau được freeze trong v0.1.

### Rule 1

CLI không parse SQL.

### Rule 2

Parser không truy cập database.

### Rule 3

Executor không trực tiếp thao tác filesystem.

### Rule 4

Storage không hiểu SQL syntax.

### Rule 5

Pager không hiểu row/table semantics.

### Rule 6

Catalog chứa metadata, không chứa row data.

### Rule 7

DatabaseEngine điều phối, không trở thành god object.

### Rule 8

Disk format phải explicit, không serialize raw C++ objects.

### Rule 9

Resources được quản lý bằng RAII.

### Rule 10

Không thêm feature ngoài scope trước khi v0.1 đạt Definition of Done.

---

# 51. Kiến trúc v0.1 sau khi hoàn thành

```text
                       TaySQL
                         │
                 ┌───────▼───────┐
                 │      CLI      │
                 └───────┬───────┘
                         │ SQL
                 ┌───────▼───────┐
                 │ DatabaseEngine│
                 └───────┬───────┘
                         │
              ┌──────────▼──────────┐
              │      SQL Frontend   │
              │                     │
              │ Lexer → Parser → AST│
              └──────────┬──────────┘
                         │
                 ┌───────▼───────┐
                 │    Executor   │
                 └────┬─────┬────┘
                      │     │
             ┌────────▼─┐ ┌─▼──────────┐
             │ Catalog  │ │  Storage   │
             └──────────┘ └─────┬──────┘
                                │
                         ┌──────▼──────┐
                         │    Pager    │
                         └──────┬──────┘
                                │
                         ┌──────▼──────┐
                         │    Disk     │
                         └─────────────┘
```

---

# 52. Hướng phát triển sau v0.1

Không thuộc scope hiện tại, nhưng architecture phải không chặn các bước sau.

### v0.2

Có thể thêm:

```text
WHERE
DELETE
UPDATE
more data types
selected columns
DROP TABLE
```

### v0.3

Có thể thêm:

```text
B+ Tree
index
row identifiers
page cache
```

### v0.4+

Có thể nghiên cứu:

```text
transactions
WAL
crash recovery
query planner
optimizer
JOIN
concurrency
```

Không cần thiết kế chi tiết các phiên bản này ở thời điểm v0.1.

---

# 53. Tuyên bố scope cuối cùng

**TaySQL 0.1 là một persistent single-process SQL database engine nhỏ, viết bằng C++20, hỗ trợ CREATE TABLE, INSERT và SELECT trên INT/TEXT, sử dụng handwritten lexer/parser, typed AST, executor, persistent catalog, heap table storage và page-based disk I/O.**

TaySQL 0.1 không hướng tới performance cao.

TaySQL 0.1 không hướng tới SQL compatibility đầy đủ.

TaySQL 0.1 không hướng tới production use.

Mục tiêu của v0.1 là:

> Có thể hiểu, implement, test và giải thích toàn bộ đường đi của một câu SQL từ chuỗi ký tự cho tới bytes được lưu trên disk.
