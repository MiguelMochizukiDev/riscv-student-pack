cmake_minimum_required(VERSION 3.15)

project(riscv_emulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
)

set(SOURCES
	src/cpu.cpp
	src/rv32i_handler.cpp
	src/memory.cpp
	src/elf.cpp
	src/main.cpp
)

add_executable(riscv_emulator ${SOURCES})

target_include_directories(riscv_emulator
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

# Use target_compile_options com generator expressions
target_compile_options(riscv_emulator PRIVATE
    $<$<CONFIG:Debug>:-g>
    $<$<CONFIG:Release>:-O2>
)