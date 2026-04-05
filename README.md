# RISC-V Student Pack

Educational toolkit for learning RISC-V in modern C++17.

This project builds two complementary tools in parallel:

- an ELF32 assembler for RV32 assembly source
- an emulator that executes produced ELF32 binaries

The intent is a tight educational feedback loop: write assembly, assemble it, run it, inspect CPU state.

> Status: work in progress. Current implementation is focused on RV32I. RV32M/RV32A/RV32C, richer directives, and formal tests are still in progress.

## Current capabilities

### Assembler (available)

- Two-pass assembly with forward and backward label resolution
- RV32I mnemonic encoding via handler abstraction
- ELF32 executable output for the bundled emulator
- Compatible output format (`ET_EXEC`, `EM_RISCV`, single `PT_LOAD` segment)

### Emulator (available)

- ELF32 loader with `PT_LOAD` segment mapping and entry-point boot
- RV32I instruction decode and execute loop
- 32 general-purpose registers (`x0`-`x31`)
- Stack pointer initialized near top of emulated memory
- Instruction trace output (`PC` and fetched instruction)
- Register dump at program end
- `ecall` and `ebreak` currently halt execution (no Linux syscall ABI yet)

### In progress

- RV32M, RV32A, RV32C support (assembler and emulator)
- Pseudoinstructions and richer assembler directives
- Linux ABI syscall handling
- Structured tests (unit + integration)

## Quick start

Prerequisites:

- Linux
- CMake 3.10+
- GCC 7+ or Clang 5+ with C++17 support

Build both tools from the repository root:

```bash
cmake -S assembler -B assembler/build
cmake --build assembler/build

cmake -S emulator -B emulator/build
cmake --build emulator/build
```

Assemble a program:

```bash
./assembler/build/riscv_assembler input.s output.elf
```

Run it in the emulator:

```bash
./emulator/build/riscv_emulator output.elf
```

## CLI reference

Assembler:

```text
riscv_assembler <input.s> <output.elf>
```

Emulator:

```text
riscv_emulator <elf>
```

## Feature roadmap

### Emulator

- [x] RV32I execution
- [ ] RV32M execution
- [ ] RV32A execution
- [ ] RV32C execution
- [x] ELF32 input loading (`PT_LOAD`, entry point)
- [x] Handler-based ISA dispatch (`InstructionHandler`)
- [x] 32 general-purpose registers
- [ ] Linux ABI syscalls (`exit`, `read`, `write`, `openat`, `close`, `fstat`, `brk`)
- [ ] Optional debug mode flag
- [ ] Error-time register dump and stack trace

### Assembler

- [x] RV32I encoding
- [ ] RV32M encoding
- [ ] RV32A encoding
- [ ] RV32C encoding
- [x] ELF32 executable output compatible with emulator
- [x] Handler-based encoding (`MnemonicHandler`)
- [x] Two-pass label resolution
- [ ] Pseudoinstruction expansion
- [ ] Section directives (`.text`, `.data`, `.bss`, `.rodata`)
- [ ] Data directives (`.byte`, `.half`, `.word`, `.ascii`, `.asciiz`, `.space`)
- [ ] Optional assembly diagnostics flag

### Testing

- [ ] Unit tests per ISA extension
- [ ] Unit tests for assembler pipeline components
- [ ] End-to-end assemble then emulate integration tests

## Architecture notes

- Emulator instruction execution is decoupled behind `InstructionHandler`, enabling ISA extensions as pluggable handlers.
- Assembler encoding is decoupled behind `MnemonicHandler`, allowing extension-specific encoders.
- This symmetry is intentional so assembler output and emulator support can evolve together.

## Project layout

```text
riscv-student-pack/
├── assembler/
│   ├── include/
│   ├── src/
│   └── CMakeLists.txt
├── emulator/
│   ├── include/
│   ├── src/
│   └── CMakeLists.txt
├── docs/
├── LICENSE
└── README.md
```

## Diagrams

Architecture diagrams are available in the `docs` directory:

- `docs/riscv_student_pack_assembler.png`
- `docs/riscv_student_pack_emulator.png`
- `docs/riscv_student_pack_full.png`

## Known limitations

- No Linux syscall emulation yet; `ecall` halts execution.
- No compressed (`C`) or multiply/divide (`M`) instruction support yet.
- Formal automated tests are not complete yet.
- Development and validation are currently Linux-centric.
