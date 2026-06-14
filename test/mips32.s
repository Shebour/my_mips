# MIPS32 additions: mul, movz, movn, clz, clo, madd, msub

.global main
main:
    # mul: 6 * 7 = 42 (3-operand, result in rd)
    ori $t0, $0, 6
    ori $t1, $0, 7
    mul $a0, $t0, $t1
    jal print_int
    jal nl

    # movz: rt == 0 -> move -> 99
    ori $t0, $0, 99
    ori $t2, $0, 0
    ori $a0, $0, 0
    movz $a0, $t0, $t2
    jal print_int
    jal nl

    # movn: rt != 0 -> move -> 77
    ori $t0, $0, 77
    ori $t2, $0, 5
    ori $a0, $0, 0
    movn $a0, $t0, $t2
    jal print_int
    jal nl

    # movz not taken: rt != 0 -> a0 stays 22
    ori $t0, $0, 11
    ori $t2, $0, 1
    ori $a0, $0, 22
    movz $a0, $t0, $t2
    jal print_int
    jal nl

    # clz: leading zeros of 0x00FF0000 = 8
    lui $t0, 0x00FF
    clz $a0, $t0
    jal print_int
    jal nl

    # clo: leading ones of 0xFFF00000 = 12
    lui $t0, 0xFFF0
    clo $a0, $t0
    jal print_int
    jal nl

    # madd: LO = 0 then += 3*4 -> 12
    mthi $0
    mtlo $0
    ori $t0, $0, 3
    ori $t1, $0, 4
    madd $t0, $t1
    mflo $a0
    jal print_int
    jal nl

    # msub: LO = 12 then -= 2*5 -> 2
    ori $t0, $0, 2
    ori $t1, $0, 5
    msub $t0, $t1
    mflo $a0
    jal print_int
    jal nl

    # maddu: LO = 0 then += 10*10 -> 100
    mthi $0
    mtlo $0
    ori $t0, $0, 10
    ori $t1, $0, 10
    maddu $t0, $t1
    mflo $a0
    jal print_int
    jal nl

    # msubu: LO = 100 then -= 8*5 -> 60
    ori $t0, $0, 8
    ori $t1, $0, 5
    msubu $t0, $t1
    mflo $a0
    jal print_int
    jal nl

    j exit

nl:
    ori $a0, $0, 10
    ori $v0, $0, 11
    syscall
    jr $ra
    nop
