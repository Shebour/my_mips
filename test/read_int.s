# print_int(read_int() + 1)

.global main
main:
    jal read_int
    ori $t1, $0, 1
    add $a0, $v0, $t1
    jal print_int
    j exit
