# Test SRAI vs SRLI
# Sets x1 = 0x80000000, then:
#   SRLI x2, x1, 4  -> x2 should be 0x08000000 (logical shift)
#   SRAI x3, x1, 4  -> x3 should be 0xF8000000 (arithmetic shift, sign-extended)
# Then verifies with ECALL

.text
_start:
    lui   x1, 0x80000       # x1 = 0x80000000
    srli  x2, x1, 4         # x2 = 0x08000000 (logical)
    srai  x3, x1, 4         # x3 = 0xF8000000 (arithmetic)
    ecall
