# Arithmetic: addi, subu, multu, div, divu (with mfhi/mflo)

.global main
main:
    # addi: 100 + (-1) = 99 (immediate is sign-extended)
    ori $t0, $0, 100
    addi $a0, $t0, -1
    jal print_int
    jal nl

    # subu: 5 - 10 = -5
    ori $t0, $0, 5
    ori $t1, $0, 10
    subu $a0, $t0, $t1
    jal print_int
    jal nl

    # multu: 6 * 7 = 42 (low word in LO)
    ori $t0, $0, 6
    ori $t1, $0, 7
    multu $t0, $t1
    mflo $a0
    jal print_int
    jal nl

    # div: 17 / 5 -> quotient 3 (LO), remainder 2 (HI)
    ori $t0, $0, 17
    ori $t1, $0, 5
    div $t0, $t1
    mflo $a0
    jal print_int
    jal nl
    mfhi $a0
    jal print_int
    jal nl

    # divu: 20 / 3 -> quotient 6 (LO), remainder 2 (HI)
    ori $t0, $0, 20
    ori $t1, $0, 3
    divu $t0, $t1
    mflo $a0
    jal print_int
    jal nl
    mfhi $a0
    jal print_int
    jal nl

    j exit

nl:
    ori $a0, $0, 10
    ori $v0, $0, 11
    syscall
    jr $ra
    nop
