# print_int(0x42 + 0x24)

.global main
main:
    ori $t0, $0, 0x42
    ori $t1, $0, 0x24
    sub $a0, $t1, $t0
    jal print_int
    j exit
