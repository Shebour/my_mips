# read_string(0x4000, 10); print_string(0x4000)

.global main
main:
    ori $a0, $0, 0x4000
    ori $a1, $0, 10
    jal read_string
    ori $a0, $0, 0x4000
    jal print_string
    j exit
