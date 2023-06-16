main:
  # print_string("Hello World!\n");
  ori $a0, $0, hello
  ori $v0, $0, 4
  syscall
# exit();
  ori $v0, $0, 10
  syscall

hello:
  .asciiz "Hello World!\n"
