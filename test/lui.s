# print_int(1 << 16)

.global main
main:
    lui $a0, 1
    jal print_int
    j exit
