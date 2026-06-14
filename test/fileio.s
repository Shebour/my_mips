# File syscalls: open, write, read, close
# Writes "Hello" to a file, reads it back and prints it.

.global main
main:
    # fd = open(fname, 1)  -- write mode
    ori $a0, $0, fname
    ori $a1, $0, 1
    jal open
    or $s0, $0, $v0

    # write(fd, msg, 5)
    or $a0, $0, $s0
    ori $a1, $0, msg
    ori $a2, $0, 5
    jal write

    # close(fd)
    or $a0, $0, $s0
    jal close

    # fd = open(fname, 0)  -- read mode
    ori $a0, $0, fname
    ori $a1, $0, 0
    jal open
    or $s0, $0, $v0

    # read(fd, buf, 5)
    or $a0, $0, $s0
    ori $a1, $0, buf
    ori $a2, $0, 5
    jal read

    # close(fd)
    or $a0, $0, $s0
    jal close

    # print_string(buf)
    ori $a0, $0, buf
    jal print_string

    j exit

fname:
    .asciiz "/tmp/my_mips_io.txt"
msg:
    .asciiz "Hello"
buf:
    .space 8
