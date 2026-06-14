# Shift instructions: sll, srl, sra, sllv, srlv, srav

.global main
main:
    # sll: 1 << 4 = 16
    ori $t0, $0, 1
    sll $a0, $t0, 4
    jal print_int
    jal nl

    # srl: 0x80 >> 4 = 8 (logical)
    ori $t0, $0, 0x80
    srl $a0, $t0, 4
    jal print_int
    jal nl

    # sra: -16 >> 2 = -4 (arithmetic)
    addi $t0, $0, -16
    sra $a0, $t0, 2
    jal print_int
    jal nl

    # sllv: 3 << 5 = 96
    ori $t0, $0, 3
    ori $t1, $0, 5
    sllv $a0, $t0, $t1
    jal print_int
    jal nl

    # srlv: 0x100 >> 4 = 16 (logical)
    ori $t0, $0, 0x100
    ori $t1, $0, 4
    srlv $a0, $t0, $t1
    jal print_int
    jal nl

    # srav: -64 >> 2 = -16 (arithmetic)
    addi $t0, $0, -64
    ori $t1, $0, 2
    srav $a0, $t0, $t1
    jal print_int
    jal nl

    j exit

nl:
    ori $a0, $0, 10
    ori $v0, $0, 11
    syscall
    jr $ra
    nop
