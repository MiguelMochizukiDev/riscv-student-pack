# Test branch instructions
# Expected: x1 = 2 (branch was taken twice)

.text
_start:
    addi  x1, x0, 0         # x1 = 0
    addi  x2, x0, 3         # x2 = 3
    addi  x3, x0, 3         # x3 = 3

loop:
    addi  x1, x1, 1         # x1++
    bne   x2, x1, loop      # loop while x2 != x1

    addi  x4, x0, 5         # x4 = 5
    addi  x5, x0, 5         # x5 = 5
    beq   x4, x5, skip      # should branch (x4 == x5)
    addi  x1, x1, 1         # should NOT execute

skip:
    blt   x0, x1, done      # x0 < x1, so branch
    addi  x1, x0, 0         # should NOT execute

done:
    ecall
