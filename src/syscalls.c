#include "syscalls.h"

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "logger.h"
#include "utils.h"

extern struct global *glob;

void print_int(void)
{
  printf("%d", (int32_t)glob->reg[A0]);
}

void print_string(void)
{
  char *value = ((char *)glob->memory) + glob->reg[A0];
  printf("%s", value);
}

void read_int(void)
{
  int value = 0;
  scanf("%d", &value);
  glob->reg[V0] = value;
}

void read_string(void)
{
  char *buf = calloc(glob->reg[A1], sizeof(char));
  ssize_t ret = read(STDIN_FILENO, buf, glob->reg[A1]);
  if (ret == -1)
  {
    perror("Fail reading string\n");
    return;
  }
  char *value = ((char *)glob->memory) + glob->reg[A0];
  strncpy(value, buf, glob->reg[A1]);
  free(buf);
}

void print_char(void)
{
  printf("%c", glob->reg[A0]);
}

void read_char(void)
{
  char value = 0;
  scanf("%c", &value);
  glob->reg[V0] = value;
}

void _open(void)
{
  const char *path = (char *)glob->memory + glob->reg[A0];
  /* Subject convention: $a1 is 0 for read, 1 for write. */
  int flags = glob->reg[A1] == 0 ? O_RDONLY : (O_WRONLY | O_CREAT | O_TRUNC);
  glob->reg[V0] = open(path, flags, 0644);
}

void _read(void)
{
  glob->reg[V0] =
      read(glob->reg[A0], (char *)glob->memory + glob->reg[A1], glob->reg[A2]);
}

void _write(void)
{
  glob->reg[V0] =
      write(glob->reg[A0], (char *)glob->memory + glob->reg[A1], glob->reg[A2]);
}

void _close(void)
{
  close(glob->reg[A0]);
}

int call_syscall(void)
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
  case 11:
    print_char();
    break;
  case 12:
    read_char();
    break;
  case 13:
    _open();
    break;
  case 14:
    _read();
    break;
  case 15:
    _write();
    break;
  case 16:
    _close();
    break;
  case 17: /* exit2: always halts, the code in $a0 is the exit status */
    return 1;
  default:
    LOG_ERROR("Unknown syscall number: %u", syscall_value);
    return 1;
  }
  return 0;
}
