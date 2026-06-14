# Overflow handling: add/sub write 1 to $k0 on signed overflow, 0 otherwise.

.global main
main:
    # add 0x7FFFFFFF + 1 -> overflow -> $k0 = 1
    lui $t0, 0x7FFF
    ori $t0, $t0, 0xFFFF
    ori $t1, $0, 1
    add $t2, $t0, $t1
    or $a0, $0, $k0
    jal print_int
    jal nl

    # add 1 + 2 -> no overflow -> result 3, $k0 = 0
    ori $t0, $0, 1
    ori $t1, $0, 2
    add $a0, $t0, $t1
    jal print_int
    jal nl
    or $a0, $0, $k0
    jal print_int
    jal nl

    # sub 0x80000000 - 1 -> overflow -> $k0 = 1
    lui $t0, 0x8000
    ori $t1, $0, 1
    sub $t2, $t0, $t1
    or $a0, $0, $k0
    jal print_int
    jal nl

    j exit

nl:
    ori $a0, $0, 10
    ori $v0, $0, 11
    syscall
    jr $ra
    nop
