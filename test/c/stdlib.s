.global print_int
print_int:
    ori $v0, $0, 801
    syscall
    jr $ra
    nop

.global print_string
print_string:
    ori $v0, $0, 804
    syscall
    jr $ra
    nop

.global read_int
read_int:
    ori $v0, $0, 805
    syscall
    jr $ra
    nop

.global read_string
read_string:
    ori $v0, $0, 808
    syscall
    jr $ra
    nop

.global exit
exit:
    ori $v0, $0, 810
    syscall
    jr $ra
    nop

.global print_char
print_char:
    ori $v0, $0, 811
    syscall
    jr $ra
    nop

.global read_char
read_char:
    ori $v0, $0, 812
    syscall
    jr $ra
    nop

.global open
open:
    ori $v0, $0, 813
    syscall
    jr $ra
    nop

.global read
read:
    ori $v0, $0, 814
    syscall
    jr $ra
    nop

.global write
write:
    ori $v0, $0, 815
    syscall
    jr $ra
    nop

.global close
close:
    ori $v0, $0, 816
    syscall
    jr $ra
    nop

.global exit2
exit2:
    ori $v0, $0, 817
    syscall
    jr $ra
    nop
