# Set-on-less-than: slt, sltu, sltiu

.global main
main:
    # slt: (-1 < 1) signed = 1
    addi $t0, $0, -1
    ori $t1, $0, 1
    slt $a0, $t0, $t1
    jal print_int
    jal nl

    # sltu: (0xFFFFFFFF < 1) unsigned = 0
    sltu $a0, $t0, $t1
    jal print_int
    jal nl

    # sltiu: (0 < 5) unsigned = 1
    sltiu $a0, $0, 5
    jal print_int
    jal nl

    j exit

nl:
    ori $a0, $0, 10
    ori $v0, $0, 11
    syscall
    jr $ra
    nop
