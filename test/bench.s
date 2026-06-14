# Arithmetic-heavy loop: ~15 ALU ops per iteration (one JIT block) then a
# branch. Used to compare interpreter vs JIT throughput.
.global main
main:
    lui   $s1, 0x0020        # ~2,097,152 iterations
    ori   $s0, $0, 1
loop:
    addiu $s0, $s0, 3
    sll   $t0, $s0, 1
    addu  $s0, $s0, $t0
    xori  $s0, $s0, 0x1234
    or    $s0, $s0, $t0
    srl   $t1, $s0, 2
    addu  $s0, $s0, $t1
    subu  $s0, $s0, $t0
    sltu  $t2, $s0, $t1
    addu  $s0, $s0, $t2
    nor   $s0, $s0, $0
    addiu $s0, $s0, 5
    sll   $s0, $s0, 1
    srl   $s0, $s0, 1
    addiu $s1, $s1, -1
    bne   $s1, $0, loop
    nop
    or    $a0, $0, $s0
    jal print_int
    j exit
