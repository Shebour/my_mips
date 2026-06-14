# Byte/halfword loads and stores: sb, lb, lbu, sh, lh, lhu
# Uses the stack as scratch memory.

.global main
main:
    # store byte 0xAB, load it back signed and unsigned
    ori $t0, $0, 0xAB
    sb $t0, -4($sp)
    lb $a0, -4($sp)        # sign-extended: 0xAB = -85
    jal print_int
    jal nl
    lbu $a0, -4($sp)       # zero-extended: 0xAB = 171
    jal print_int
    jal nl

    # store halfword 0x8765, load it back signed and unsigned
    ori $t1, $0, 0x8765
    sh $t1, -8($sp)
    lh $a0, -8($sp)        # sign-extended: 0x8765 = -30875
    jal print_int
    jal nl
    lhu $a0, -8($sp)       # zero-extended: 0x8765 = 34661
    jal print_int
    jal nl

    j exit

nl:
    ori $a0, $0, 10
    ori $v0, $0, 11
    syscall
    jr $ra
    nop
