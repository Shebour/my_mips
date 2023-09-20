# print_int(0x42 - 0x24)

.global main
main:
    ori $t0, $0, 0xFFFF
    ori $t1, $0, 0xFFFF
    #sub $a0, $t1, $t0
    xor $a0, $a0
    add $a0, $t0, $t1
    add $a0, $a0, $t1
    sll $a0, $a0, 26
    jal print_int
    j exit
