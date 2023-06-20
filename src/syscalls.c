#include "syscalls.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "utils.h"

extern struct global *glob;

void print_int()
{
  printf("%u", glob->reg[A0]);
}

void print_string()
{
  char *value = ((char *)glob->memory) + glob->reg[A0];
  printf("%s", value);
}

void read_int()
{
  int value = 0;
  scanf("%d", &value);
  glob->reg[V0] = value;
}

void read_string()
{
  char *buf = calloc(glob->reg[A1], sizeof(char));
  ssize_t ret = read(STDIN_FILENO, buf, glob->reg[A1]);
  if (ret == -1)
  {
    perror("Fail reading string\n");
    return;
  }
}

int call_syscall()
{
  uint32_t syscall_value = glob->reg[V0];
  switch (syscall_value)
  {
  case 1:
    print_int();
    break;
  case 4:
    print_string();
    break;
  case 5:
    read_int();
    break;
  case 8:
    read_string();
    break;
  case 10:
    return 1;
  }
  return 0;
}
