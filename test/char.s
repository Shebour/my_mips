# Character syscalls: print_char (11) and read_char (12)

.global main
main:
    # print 'A', 'B', newline
    ori $a0, $0, 65
    ori $v0, $0, 11
    syscall
    ori $a0, $0, 66
    ori $v0, $0, 11
    syscall
    ori $a0, $0, 10
    ori $v0, $0, 11
    syscall

    # read one char and echo it
    ori $v0, $0, 12
    syscall
    or $a0, $0, $v0
    ori $v0, $0, 11
    syscall

    j exit
