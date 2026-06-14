# A straight-line sampler of many instruction kinds. Run with -l it makes the
# disassembler emit one of almost every mnemonic, for coverage. The computed
# values are meaningless; only the decode/disassemble paths matter.

.global main
main:
    addu  $t0, $t1, $t2
    subu  $t0, $t1, $t2
    and   $t0, $t1, $t2
    or    $t0, $t1, $t2
    xor   $t0, $t1, $t2
    nor   $t0, $t1, $t2
    slt   $t0, $t1, $t2
    sltu  $t0, $t1, $t2
    sll   $t0, $t1, 3
    srl   $t0, $t1, 3
    sra   $t0, $t1, 3
    sllv  $t0, $t1, $t2
    srlv  $t0, $t1, $t2
    srav  $t0, $t1, $t2
    movz  $t0, $t1, $t2
    movn  $t0, $t1, $t2
    mult  $t1, $t2
    multu $t1, $t2
    mthi  $t1
    mtlo  $t2
    mfhi  $t0
    mflo  $t0
    mul   $t0, $t1, $t2
    madd  $t1, $t2
    maddu $t1, $t2
    msub  $t1, $t2
    msubu $t1, $t2
    clz   $t0, $t1
    clo   $t0, $t1
    addiu $t0, $t1, 5
    andi  $t0, $t1, 5
    xori  $t0, $t1, 5
    slti  $t0, $t1, 5
    sltiu $t0, $t1, 5
    lui   $t0, 5
    sb    $t0, 0($sp)
    lb    $t0, 0($sp)
    lbu   $t0, 0($sp)
    sh    $t0, 0($sp)
    lh    $t0, 0($sp)
    lhu   $t0, 0($sp)
    sw    $t0, 0($sp)
    lw    $t0, 0($sp)
    j exit
