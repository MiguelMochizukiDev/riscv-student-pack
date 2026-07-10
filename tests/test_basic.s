# Test basic ALU and I-type instructions
# Expected final state:
#   x1 = 0x0000000F
#   x2 = 0x00000005
#   x3 = 0x0000000A
#   x4 = 0x00000001 (slti: 5 < 10)
#   x5 = 0x00000001 (sltiu: 5 < 10 unsigned)
#   x6 = 0x000000FF (ori)
#   x7 = 0x000000F0 (andi)

.text
_start:
    addi  x1, x0, 15        # x1 = 15
    addi  x2, x0, 5         # x2 = 5
    add   x3, x2, x2        # x3 = 10
    slti  x4, x2, 10        # x4 = 1 (5 < 10 signed)
    sltiu x5, x2, 10        # x5 = 1 (5 < 10 unsigned)
    ori   x6, x1, 0xFF      # x6 = 0xFF
    andi  x7, x6, 0xF0      # x7 = 0xF0
    ecall
