# Test load/store instructions
# Writes values to stack, then reads them back

.text
_start:
    addi  sp, x0, 0x7F0     # sp = 0x7F0
    addi  x1, x0, 0x7B      # x1 = 0x7B
    addi  x2, x0, 0x234     # x2 = 0x234
    lui   x3, 0xDEADB       # x3 = 0xDEADB000
    ori   x3, x3, 0x0FF     # x3 = 0xDEADB0FF

    sw    x3, 0(sp)         # store word at 0x7F0
    sh    x2, 4(sp)         # store half at 0x7F4
    sb    x1, 8(sp)         # store byte at 0x7F8

    lw    x4, 0(sp)         # x4 = 0xDEADB0FF
    lhu   x5, 4(sp)         # x5 = 0x0234
    lbu   x6, 8(sp)         # x6 = 0x7B

    # Test sign extension on lb/lh
    lb    x7, 8(sp)         # x7 = 0x0000007B (sign bit clear)
    lh    x8, 4(sp)         # x8 = 0x00000234 (sign bit clear)

    ecall
