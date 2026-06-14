# Logical instructions: and, or, xor, nor, andi, xori

.global main
main:
    # and: 0xF0 & 0x3C = 0x30 = 48
    ori $t0, $0, 0xF0
    ori $t1, $0, 0x3C
    and $a0, $t0, $t1
    jal print_int
    jal nl

    # or: 0xF0 | 0x0C = 0xFC = 252
    ori $t0, $0, 0xF0
    ori $t1, $0, 0x0C
    or $a0, $t0, $t1
    jal print_int
    jal nl

    # xor: 0xFF ^ 0x0F = 0xF0 = 240
    ori $t0, $0, 0xFF
    ori $t1, $0, 0x0F
    xor $a0, $t0, $t1
    jal print_int
    jal nl

    # nor: ~(0x0F | 0xF0) = 0xFFFFFF00 = -256
    ori $t0, $0, 0x0F
    ori $t1, $0, 0xF0
    nor $a0, $t0, $t1
    jal print_int
    jal nl

    # andi: 0xFF & 0x0F = 15
    ori $t0, $0, 0xFF
    andi $a0, $t0, 0x0F
    jal print_int
    jal nl

    # xori: 0xFF ^ 0xF0 = 0x0F = 15
    ori $t0, $0, 0xFF
    xori $a0, $t0, 0xF0
    jal print_int
    jal nl

    j exit

nl:
    ori $a0, $0, 10
    ori $v0, $0, 11
    syscall
    jr $ra
    nop
