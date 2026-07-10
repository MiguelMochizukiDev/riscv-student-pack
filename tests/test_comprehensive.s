# Comprehensive RV32I test
# Exercises all instruction types with SRAI bug verification

.text
_start:
    # ===== R-TYPE =====
    addi  x1, x0, 10        # x1 = 10
    addi  x2, x0, 3         # x2 = 3
    add   x3, x1, x2        # x3 = 13
    sub   x4, x1, x2        # x4 = 7
    sll   x5, x1, x2        # x5 = 10 << 3 = 80
    slt   x6, x2, x1        # x6 = 1 (3 < 10)
    sltu  x7, x2, x1        # x7 = 1 (3 < 10 unsigned)
    xor   x8, x1, x2        # x8 = 10 ^ 3 = 9
    srl   x9, x1, x2        # x9 = 10 >> 3 = 1
    or    x10, x1, x2       # x10 = 10 | 3 = 11
    and   x11, x1, x2       # x11 = 10 & 3 = 2

    # ===== SRAI vs SRLI VERIFICATION =====
    lui   x12, 0x80000      # x12 = 0x80000000
    srli  x13, x12, 4       # x13 = 0x08000000 (logical)
    srai  x14, x12, 4       # x14 = 0xF8000000 (arithmetic - was broken before fix)

    # ===== LOAD/STORE =====
    addi  sp, x0, 0x7C0     # sp = 0x7C0
    sw    x1, 0(sp)         # store x1
    lw    x15, 0(sp)        # x15 = x1

    # ===== BRANCH =====
    addi  x16, x0, 0        # x16 = 0
    addi  x17, x0, 5        # x17 = 5
branch_loop:
    addi  x16, x16, 1       # x16++
    bne   x16, x17, branch_loop  # branch until x16 == 5

    # ===== JAL/JALR =====
    addi  x18, x0, 1        # x18 = 1
    jal   x19, skip_jal     # jal with explicit rd
    addi  x18, x18, 100     # should NOT execute

skip_jal:
    addi  x18, x18, 2       # x18 = 3

    # ===== LUI/AUIPC =====
    lui   x20, 0x12345      # x20 = 0x12345000
    auipc x21, 0            # x21 = current PC

    ecall
