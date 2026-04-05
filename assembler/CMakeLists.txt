cmake_minimum_required(VERSION 3.15)
project(riscv_assembler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(SOURCES
    src/assembler.cpp
    src/elf_writer.cpp
    src/lexer.cpp
    src/mnemonic_handler.cpp
    src/parser.cpp
    src/main.cpp
)

add_executable(riscv_assembler ${SOURCES})

target_include_directories(riscv_assembler
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_options(riscv_assembler PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    $<$<CONFIG:Debug>:-g>
    $<$<CONFIG:Release>:-O2>
)
