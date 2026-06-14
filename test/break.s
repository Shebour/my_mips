# The break instruction must stop execution immediately.

.global main
main:
    # print 'A'
    ori $a0, $0, 65
    ori $v0, $0, 11
    syscall

    break          # halts here

    # this must never run
    ori $a0, $0, 66
    ori $v0, $0, 11
    syscall

    j exit
