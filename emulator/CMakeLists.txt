cmake_minimum_required(VERSION 3.15)
project(riscv_emulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(SOURCES
    src/cpu.cpp
    src/rv32i_handler.cpp
    src/memory.cpp
    src/elf.cpp
    src/emulator.cpp
    src/main.cpp
)

add_executable(riscv_emulator ${SOURCES})

target_include_directories(riscv_emulator
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_options(riscv_emulator PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    $<$<CONFIG:Debug>:-g>
    $<$<CONFIG:Release>:-O2>
)