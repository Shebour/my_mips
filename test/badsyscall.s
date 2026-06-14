# An unrecognized syscall number must report an error and halt.

.global main
main:
    ori $v0, $0, 99
    syscall
    j exit
