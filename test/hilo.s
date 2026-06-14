# Special registers: mthi/mfhi and mtlo/mflo round-trip

.global main
main:
    # mthi then mfhi: 0x1234 = 4660
    ori $t0, $0, 0x1234
    mthi $t0
    mfhi $a0
    jal print_int
    jal nl

    # mtlo then mflo: 0x5678 = 22136
    ori $t1, $0, 0x5678
    mtlo $t1
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
