# Unaligned loads and stores: swl, swr, lwl, lwr
# Uses the stack as scratch memory.

.global main
main:
    # swr + swl write a full word at an aligned address; lw reads it back.
    # 0xDEADBEEF = -559038737
    lui $t1, 0xDEAD
    ori $t1, $t1, 0xBEEF
    swr $t1, -8($sp)
    swl $t1, -5($sp)
    lw $a0, -8($sp)
    jal print_int
    jal nl

    # Store two adjacent words, then read an unaligned word straddling them.
    #   mem:  A = 0x12345678, A+4 = 0x9ABCDEF0
    #   little-endian word at A+2 = 0xDEF01234 = -554658764
    lui $t2, 0x1234
    ori $t2, $t2, 0x5678
    sw $t2, -16($sp)
    lui $t3, 0x9ABC
    ori $t3, $t3, 0xDEF0
    sw $t3, -12($sp)
    ori $a0, $0, 0
    lwr $a0, -14($sp)      # A+2
    lwl $a0, -11($sp)      # A+5
    jal print_int
    jal nl

    j exit

nl:
    ori $a0, $0, 10
    ori $v0, $0, 11
    syscall
    jr $ra
    nop
