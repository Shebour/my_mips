#include "dbg.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "cpu.h"
#include "utils.h"

extern struct global *glob;

int get_from_user(char *buffer, size_t size)
{
  size_t cnt = 0;
  char c;

  if(buffer == NULL || size == 0)
    return 0;

  while(read(STDIN_FILENO, &c, 1) == 1 && cnt < size - 1)
  {
    if(c == '\n')
    {
      buffer[cnt] = 0;
      return 1;
    }

    buffer[cnt++] = c;
  }

  buffer[cnt] = 0;
  return 1;
}

void print_registers(void)
{
  printf("pc = 0x%08x\n", glob->pc);
  printf("hi = 0x%08x\n", glob->hi);
  printf("lo = 0x%08x\n", glob->lo);
  for (int i = 0; i < NB_REG; i++)
  {
    printf("r%d = 0x%08x\n", i, glob->reg[i]);
  }
}

void debug(void)
{
  printf("dbg>");
  uint32_t *instru = ((uint32_t *)glob->memory) + (glob->pc / 4);
  char buf[100] = { 0 };
  if (!get_from_user(buf, 100))
  {
    fprintf(stderr, "Cannot get from user\n");
    return;
  }
  if (!strcmp(buf, "registers"))
    print_registers();

  if (!strcmp(buf, "start"))
  {
    int ret = exec_inst(instru);
    if (ret)
    {
      fprintf(stderr, "Error during execution\n");
    }
  }
  if (!strcmp(buf, "help"))
  {
    fprintf(stderr, "registers          | print registers\n");
    fprintf(stderr, "print <pc_address> | print instruction at specified pc_address\n");
    fprintf(stderr, "break <pc_address> | set breakpoint\n");
    fprintf(stderr, "continue           | run until breakpoint or end\n");
    fprintf(stderr, "exit               | exit program\n");
    fprintf(stderr, "step               | execute next instruction\n");
    fprintf(stderr, "start              | set breakpoint at first instruction and start program\n");
  }
  return;
}
