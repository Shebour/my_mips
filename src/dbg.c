#include "dbg.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cpu.h"
#include "cstream.h"
#include "error.h"
#include "utils.h"
#include "vec.h"

extern struct global *glob;

void print_registers(void)
{
  printf("pc  = 0x%08x\n", glob->pc);
  printf("hi  = 0x%08x\n", glob->hi);
  printf("lo  = 0x%08x\n", glob->lo);
  for (int i = 0; i < NB_REG; i++)
  {
    printf("r%-2d = 0x%08x\n", i, glob->reg[i]);
  }
}

enum error get_content(struct cstream *cs, struct vec *line)
{
  enum error err;

  while (true)
  {
    int c;
    if ((err = cstream_pop(cs, &c)))
      return err;

    if (c == EOF || c == '\n')
    {
      vec_push(line, 0);
      break;
    }
    vec_push(line, c);
  }
  return NO_ERROR;
}

int parse_one_arg(struct vec *line)
{
  struct cstream *cs;

  if (isatty(STDIN_FILENO))
    cs = cstream_readline_create();
  else
    cs = cstream_file_create(stdin, false);

  vec_init(line);
  enum error er = get_content(cs, line);

  cstream_free(cs);
  if (er != NO_ERROR)
    return 0;
  return 1;
}

void debug(void)
{
  struct vec *buf = calloc(1, sizeof(struct vec));
  while (1)
  {
    if (!parse_one_arg(buf))
      break;
    if (!buf || !buf->data)
    {
      fprintf(stderr, "Error during parsing\n");
      break;
    }
    if (strlen(buf->data) == 0)
      break;
    if (!strcmp(buf->data, "registers"))
      print_registers();
    else if (!strcmp(buf->data, "start"))
    {
      uint32_t *instru = ((uint32_t *)glob->memory) + (glob->pc / 4);
      int ret = exec_inst(instru);
      if (ret == 1)
      {
        fprintf(stderr, "Error during execution\n");
        break;
      }
      if (ret == 2)
        break;
    }
    else if (!strcmp(buf->data, "step"))
    {
      uint32_t *instru = ((uint32_t *)glob->memory) + (glob->pc / 4);
      int ret = exec_inst(instru);
      if (ret == 1)
      {
        fprintf(stderr, "Error during execution\n");
        break;
      }
      if (ret == 2)
        break;
    }
    else if (!strcmp(buf->data, "exit"))
    {
      break;
    }
    else if (!strcmp(buf->data, "help"))
    {
      fprintf(stderr, "--------------------\n");
      fprintf(stderr, "registers          | print registers\n");
      fprintf(stderr,
          "print <pc_address> | print instruction at specified pc_address\n");
      fprintf(stderr, "break <pc_address> | set breakpoint\n");
      fprintf(stderr, "continue           | run until breakpoint or end\n");
      fprintf(stderr, "exit               | exit program\n");
      fprintf(stderr, "step               | execute next instruction\n");
      fprintf(stderr,
          "start              | set breakpoint at first instruction and "
          "start program\n");
      fprintf(stderr, "--------------------\n");
    }
    vec_destroy(buf);
  }
  free(buf);
  return;
}
