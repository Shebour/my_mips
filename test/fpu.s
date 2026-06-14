# Floating-point (COP1): mtc1/mfc1, add.s/mul.s, cvt, compare+bc1t, add.d,
# and lwc1/swc1. Each result is converted to an int and printed.

.global main
main:
    # f0 = 3.0 (0x40400000), f1 = 4.0 (0x40800000)
    lui  $t0, 0x4040
    mtc1 $t0, $f0
    lui  $t0, 0x4080
    mtc1 $t0, $f1

    # add.s: 3.0 + 4.0 = 7.0 -> 7
    add.s   $f2, $f0, $f1
    cvt.w.s $f3, $f2
    mfc1    $a0, $f3
    jal print_int
    jal nl

    # mul.s: 3.0 * 4.0 = 12.0 -> 12
    mul.s   $f2, $f0, $f1
    cvt.w.s $f3, $f2
    mfc1    $a0, $f3
    jal print_int
    jal nl

    # cvt.s.w then back: int 10 -> 10.0 -> 10
    ori     $t0, $0, 10
    mtc1    $t0, $f4
    cvt.s.w $f5, $f4
    cvt.w.s $f6, $f5
    mfc1    $a0, $f6
    jal print_int
    jal nl

    # c.lt.s + bc1t: 3.0 < 4.0 is true -> print 1
    c.lt.s  $f0, $f1
    bc1t    L1
    nop
    ori     $a0, $0, 0
    j       P1
L1:
    ori     $a0, $0, 1
P1:
    jal print_int
    jal nl

    # double add.d: 1.5 + 2.5 = 4.0 -> 4
    mtc1 $0,  $f10
    lui  $t0, 0x3FF8
    mtc1 $t0, $f11
    mtc1 $0,  $f12
    lui  $t0, 0x4004
    mtc1 $t0, $f13
    add.d   $f14, $f10, $f12
    cvt.w.d $f16, $f14
    mfc1    $a0, $f16
    jal print_int
    jal nl

    # swc1/lwc1 round trip of 12.0 -> 12
    swc1    $f2, -4($sp)
    lwc1    $f20, -4($sp)
    cvt.w.s $f21, $f20
    mfc1    $a0, $f21
    jal print_int
    jal nl

    # sub.s: 4.0 - 3.0 = 1.0 -> 1
    sub.s   $f2, $f1, $f0
    cvt.w.s $f3, $f2
    mfc1    $a0, $f3
    jal print_int
    jal nl

    # div.s: 12.0 / 4.0 = 3.0 -> 3
    mul.s   $f7, $f0, $f1
    div.s   $f2, $f7, $f1
    cvt.w.s $f3, $f2
    mfc1    $a0, $f3
    jal print_int
    jal nl

    # neg.s then abs.s: |-(3.0)| = 3.0 -> 3
    neg.s   $f8, $f0
    abs.s   $f9, $f8
    cvt.w.s $f3, $f9
    mfc1    $a0, $f3
    jal print_int
    jal nl

    # sqrt.s: sqrt(16.0) = 4.0 -> 4
    mul.s   $f8, $f1, $f1
    sqrt.s  $f9, $f8
    cvt.w.s $f3, $f9
    mfc1    $a0, $f3
    jal print_int
    jal nl

    # mov.s + cvt.d.s: 3.0 -> double -> 3
    mov.s   $f8, $f0
    cvt.d.s $f24, $f8
    cvt.w.d $f3, $f24
    mfc1    $a0, $f3
    jal print_int
    jal nl

    # c.eq.s false + bc1f taken: 3.0 == 4.0 false -> print 1
    c.eq.s  $f0, $f1
    bc1f    L2
    nop
    ori     $a0, $0, 0
    j       P2
L2:
    ori     $a0, $0, 1
P2:
    jal print_int
    jal nl

    # sdc1/ldc1 double round trip of 4.0 -> 4
    sdc1    $f14, -8($sp)
    ldc1    $f22, -8($sp)
    cvt.w.d $f3, $f22
    mfc1    $a0, $f3
    jal print_int
    jal nl

    # sub.d: 2.5 - 1.5 = 1.0 -> 1
    sub.d   $f18, $f12, $f10
    cvt.w.d $f3, $f18
    mfc1    $a0, $f3
    jal print_int
    jal nl

    j exit

nl:
    ori $a0, $0, 10
    ori $v0, $0, 11
    syscall
    jr $ra
    nop
