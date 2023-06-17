# print_string("Hello World!\n")

.global main
main:
    ori $a0, $0, hello
    jal print_string

    j exit

hello:
    .asciiz "Hello World!\n"
