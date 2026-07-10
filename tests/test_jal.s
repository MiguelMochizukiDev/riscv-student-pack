# Test JAL and JALR instructions
# JAL target (1 operand) -> uses ra (x1) as link register

.text
_start:
    addi  x2, x0, 1         # x2 = 1
    jal   func               # jal target (pseudo, uses x1 as link)
    addi  x2, x2, 10        # x2 += 10 (back from func)
    jal   x3, end            # jal with explicit rd

func:
    addi  x2, x2, 2         # x2 = 3
    jalr  x0, x1, 0         # return (jalr x0, x1, 0)

end:
    ecall
