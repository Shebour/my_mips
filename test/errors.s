# Exercises the error/edge paths: addi overflow and divide-by-zero.

.global main
main:
    # addi overflow: 0x7FFFFFFF + 1 -> $k0 = 1, $t1 unchanged
    lui $t0, 0x7FFF
    ori $t0, $t0, 0xFFFF
    addi $t1, $t0, 1
    or $a0, $0, $k0
    jal print_int
    jal nl

    # div / divu by zero: HI/LO left unchanged, must not crash. Emitted as
    # raw words so the assembler does not wrap them in a break-on-zero check.
    ori $t0, $0, 5
    ori $t1, $0, 0
    .word 0x0109001a   # div  $t0, $t1
    .word 0x0109001b   # divu $t0, $t1

    # sentinel to confirm execution continued
    ori $a0, $0, 7
    jal print_int
    jal nl

    j exit

nl:
    ori $a0, $0, 10
    ori $v0, $0, 11
    syscall
    jr $ra
    nop
