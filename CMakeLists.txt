cmake_minimum_required(VERSION 3.20)

project(TaySQL
    VERSION 0.1.0
    DESCRIPTION "Embedded mini SQL database engine written in C++20"
    LANGUAGES CXX
)

# Yêu cầu chuẩn C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Tự động xuất compile_commands.json cho VS Code / clangd IntelliSense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Thiết lập cờ cảnh báo compiler
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Tự động quét các file mã nguồn trong thư mục src/
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# Target thực thi chính
add_executable(taysql ${SOURCES})

# Thư mục chứa header files
target_include_directories(taysql PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
