# A tiny game running on the emulated SDL framebuffer.
#   - clears the 160x120 ARGB screen to a dark blue each frame
#   - draws a 12x12 orange block at (x, y)
#   - flushes the frame, then reads a key: w/a/s/d move, q quits.
# The window close button also quits (the device maps SDL_QUIT to 'q').

# MMIO addresses (see include/device.h)
#   FB    = 0x0F000000   framebuffer (160*120 ARGB pixels)
#   FLUSH = 0x0F0F0000   store -> present a frame
#   INPUT = 0x0F0F0004   load  -> last key (0 if none)

.global main
main:
    ori   $s0, $0, 74        # x
    ori   $s1, $0, 54        # y

loop:
    # --- clear screen to 0xFF101830 ---
    lui   $t0, 0x0F00        # t0 = framebuffer base
    lui   $t2, 0xFF10
    ori   $t2, $t2, 0x1830   # bg colour
    ori   $t3, $0, 19200     # 160*120 pixels
    ori   $t4, $0, 0
clr:
    sll   $t5, $t4, 2
    addu  $t5, $t0, $t5
    sw    $t2, 0($t5)
    addiu $t4, $t4, 1
    bne   $t4, $t3, clr
    nop

    # --- draw 12x12 block of 0xFFFFA000 at (x, y) ---
    lui   $t2, 0xFFFF
    ori   $t2, $t2, 0xA000   # player colour
    ori   $t6, $0, 0         # row
brow:
    ori   $t7, $0, 0         # col
bcol:
    addu  $t8, $s1, $t6      # y + row
    ori   $t9, $0, 160
    mul   $t8, $t8, $t9      # (y+row)*160
    addu  $t8, $t8, $s0      # + x
    addu  $t8, $t8, $t7      # + col
    sll   $t8, $t8, 2        # * 4 bytes
    addu  $t8, $t0, $t8
    sw    $t2, 0($t8)
    addiu $t7, $t7, 1
    ori   $t9, $0, 12
    bne   $t7, $t9, bcol
    nop
    addiu $t6, $t6, 1
    ori   $t9, $0, 12
    bne   $t6, $t9, brow
    nop

    # --- present frame ---
    lui   $t1, 0x0F0F
    sw    $0, 0($t1)         # FLUSH

    # --- read input ---
    lw    $v0, 4($t1)        # INPUT

    ori   $t9, $0, 113       # 'q' -> quit
    beq   $v0, $t9, done
    nop

    ori   $t9, $0, 119       # 'w' -> up (y -= 4 if y >= 4)
    bne   $v0, $t9, c_s
    nop
    slti  $t8, $s1, 4
    bne   $t8, $0, loop
    nop
    addiu $s1, $s1, -4
    j     loop
    nop
c_s:
    ori   $t9, $0, 115       # 's' -> down (y += 4 if y <= 104)
    bne   $v0, $t9, c_a
    nop
    slti  $t8, $s1, 105
    beq   $t8, $0, loop
    nop
    addiu $s1, $s1, 4
    j     loop
    nop
c_a:
    ori   $t9, $0, 97        # 'a' -> left (x -= 4 if x >= 4)
    bne   $v0, $t9, c_d
    nop
    slti  $t8, $s0, 4
    bne   $t8, $0, loop
    nop
    addiu $s0, $s0, -4
    j     loop
    nop
c_d:
    ori   $t9, $0, 100       # 'd' -> right (x += 4 if x <= 144)
    bne   $v0, $t9, loop
    nop
    slti  $t8, $s0, 145
    beq   $t8, $0, loop
    nop
    addiu $s0, $s0, 4
    j     loop
    nop

done:
    j exit
    nop
