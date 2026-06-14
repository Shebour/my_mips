# Branches: bne, bgtz, blez, bltz, bgez (taken and not-taken paths)
# Each block prints 1 when the branch is taken, 0 otherwise.

.global main
main:
    # bne 5, 3 -> taken -> 1
    ori $a0, $0, 1
    ori $t0, $0, 5
    ori $t1, $0, 3
    bne $t0, $t1, p1
    nop
    ori $a0, $0, 0
p1:
    jal print_int
    jal nl

    # bne 5, 5 -> not taken -> 0
    ori $a0, $0, 1
    ori $t0, $0, 5
    ori $t1, $0, 5
    bne $t0, $t1, p2
    nop
    ori $a0, $0, 0
p2:
    jal print_int
    jal nl

    # bgtz 5 -> taken -> 1
    ori $a0, $0, 1
    ori $t0, $0, 5
    bgtz $t0, p3
    nop
    ori $a0, $0, 0
p3:
    jal print_int
    jal nl

    # bgtz -1 -> not taken -> 0
    ori $a0, $0, 1
    addi $t0, $0, -1
    bgtz $t0, p4
    nop
    ori $a0, $0, 0
p4:
    jal print_int
    jal nl

    # blez 0 -> taken -> 1
    ori $a0, $0, 1
    ori $t0, $0, 0
    blez $t0, p5
    nop
    ori $a0, $0, 0
p5:
    jal print_int
    jal nl

    # bltz -1 -> taken -> 1
    ori $a0, $0, 1
    addi $t0, $0, -1
    bltz $t0, p6
    nop
    ori $a0, $0, 0
p6:
    jal print_int
    jal nl

    # bgez 0 -> taken -> 1
    ori $a0, $0, 1
    ori $t0, $0, 0
    bgez $t0, p7
    nop
    ori $a0, $0, 0
p7:
    jal print_int
    jal nl

    j exit

nl:
    ori $a0, $0, 10
    ori $v0, $0, 11
    syscall
    jr $ra
    nop
