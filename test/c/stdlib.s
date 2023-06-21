.global print_int
print_int:
    ori $v0, $0, 1
    syscall
    jr $ra
    nop

.global print_string
print_string:
    ori $v0, $0, 4
    syscall
    jr $ra
    nop

.global read_int
read_int:
    ori $v0, $0, 5
    syscall
    jr $ra
    nop

.global read_string
read_string:
    ori $v0, $0, 8
    syscall
    jr $ra
    nop

.global exit
exit:
    ori $v0, $0, 10
    syscall
    jr $ra
    nop

.global print_char
print_char:
    ori $v0, $0, 11
    syscall
    jr $ra
    nop

.global read_char
read_char:
    ori $v0, $0, 12
    syscall
    jr $ra
    nop

.global open
open:
    ori $v0, $0, 13
    syscall
    jr $ra
    nop

.global read
read:
    ori $v0, $0, 14
    syscall
    jr $ra
    nop

.global write
write:
    ori $v0, $0, 15
    syscall
    jr $ra
    nop

.global close
close:
    ori $v0, $0, 16
    syscall
    jr $ra
    nop

.global exit2
exit2:
    ori $v0, $0, 17
    syscall
    jr $ra
    nop
