    .text
    .align 2

# Jump to the program entry point

    .type start, @function
    .global start
start:
    jal main
    ori $v0, $zero, 810
    syscall
