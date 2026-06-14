# exit2 (syscall 17) must stop execution immediately.

.global main
main:
    # print 'A'
    ori $a0, $0, 65
    ori $v0, $0, 11
    syscall

    # exit2 with status 0 -> halts here
    ori $a0, $0, 0
    ori $v0, $0, 17
    syscall

    # this must never run
    ori $a0, $0, 66
    ori $v0, $0, 11
    syscall

    j exit
