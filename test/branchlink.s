# Branch/jump-and-link: jalr, bgezal, bltzal
# Each calls "func", which prints $a0 then a newline and returns via $ra.
# func uses no jal, so it does not clobber the return address.

.global main
main:
    # jalr: call func through a register, prints 7
    ori $a0, $0, 7
    ori $t0, $0, func
    jalr $t0
    nop

    # bgezal: 0 >= 0 -> link and call func, prints 8
    ori $a0, $0, 8
    ori $t1, $0, 0
    bgezal $t1, func
    nop

    # bltzal: -1 < 0 -> link and call func, prints 9
    ori $a0, $0, 9
    addi $t2, $0, -1
    bltzal $t2, func
    nop

    j exit

func:
    ori $v0, $0, 1
    syscall
    ori $a0, $0, 10
    ori $v0, $0, 11
    syscall
    jr $ra
    nop
