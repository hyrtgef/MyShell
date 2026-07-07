cmake_minimum_required(VERSION 3.10)

project(MyShell)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

set(SOURCES
    src/main.cpp
    src/shell.cpp
    src/parser.cpp
    src/builtines.cpp
)

add_executable(myshell ${SOURCES})

target_compile_options(myshell PRIVATE -Wall -Wextra)
