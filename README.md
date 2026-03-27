# RISC-V Student Pack - Prototype

Educational toolkit for learning RISC-V, written in modern C++17.

The end goal is two complementary tools: an **ELF32 assembler** translating RV32IMAC assembly to binary, and an **emulator** executing those binaries. They are being built together so that the assembler's output is always a valid target for the emulator.

> **Status:** Work in progress. Only the emulator exists at this stage, implementing RV32I. The remaining extensions, the assembler, and the test suite are all planned.

## Target feature set

### Emulator

- [x] **RV32I** - base integer instruction set
- [ ] **RV32M** - multiply and divide instructions
- [ ] **RV32A** - atomic memory operations
- [ ] **RV32C** - compressed (16-bit) instructions
- [x] ELF32 loading (`PT_LOAD` segments, entry point)
- [x] 32 general-purpose registers with ABI names
- [ ] Linux ABI syscalls (`exit`, `read`, `write`, `openat`, `close`, `fstat`, `brk`)
- [ ] `--debug` flag for instruction-level tracing
- [ ] Register dump and stack trace on error

### Assembler

- [ ] All RV32IMAC instructions
- [ ] Pseudoinstruction expansion
- [ ] Section directives (`.text`, `.data`, `.bss`, `.rodata`)
- [ ] Data directives (`.byte`, `.half`, `.word`, `.ascii`, `.asciiz`, `.space`)
- [ ] Forward and backward label resolution (two-pass)
- [ ] ELF32 binary output compatible with the emulator
- [ ] `--debug` flag for assembly diagnostics

### Testing

- [ ] Unit tests for each ISA extension
- [ ] Unit tests for assembler components
- [ ] Integration tests (assemble -> emulate end-to-end)

## What it does right now

Loads a 32-bit ELF binary into a flat 1 MiB memory space and runs it through a fetch-decode-execute loop implementing **RV32I**.

- ELF loading (`PT_LOAD` segments, entry point extraction)
- 32 general-purpose registers (`x0`-`x31`)
- Stack pointer initialized to top of memory
- Full RV32I instruction decoding and execution
- `ecall`/`ebreak` halt the emulator (no syscall handling yet)
- Register dump on exit
- Bounds-checked memory (throws on out-of-range access)

## Project structure

```
riscv-student-pack
├── emulator
│   ├── CMakeLists.txt
│   ├── include
│   │   ├── cpu.hpp
│   │   ├── elf.hpp
│   │   ├── emulator.hpp
│   │   ├── instruction_handler.hpp
│   │   ├── memory.hpp
│   │   └── rv32i_handler.hpp
│   └── src
│       ├── cpu.cpp
│       ├── elf.cpp
│       ├── emulator.cpp
│       ├── main.cpp
│       ├── memory.cpp
│       └── rv32i_handler.cpp
├── LICENSE
└── README.md
```

The ISA decoding is isolated behind an `InstructionHandler` interface - each extension (M, A, C) will be added as a separate handler without touching the core fetch-execute loop.

## Building

Requires a C++17-capable compiler (GCC 7+ or Clang 5+) and CMake 3.10+.

```bash
cd emulator
cmake -S . -B build
cmake --build build
```

The binary is at `build/riscv_emulator`.

## Usage

```bash
./build/riscv_emulator <elf-binary>
```

The emulator prints each fetched instruction and its PC, then dumps all registers at exit. To produce a compatible ELF binary right now you need an external cross-compiler toolchain, e.g.:

```bash
riscv32-unknown-elf-gcc -nostdlib -march=rv32imac -mabi=ilp32 -o program.elf program.s
```

Once the assembler is implemented, that step will be replaced by:

```bash
./riscv_assembler program.s program.elf
./build/riscv_emulator program.elf
```

## Requirements

- GCC or Clang with C++17 support
- CMake >= 3.10
- Linux (untested elsewhere)
