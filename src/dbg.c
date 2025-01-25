#include "dbg.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cpu.h"
#include "cstream.h"
#include "error.h"
#include "logger.h"
#include "utils.h"
#include "vec.h"

extern struct global *glob;

void print_registers(void)
{
  printf("pc  = 0x%08x\n", glob->pc);
  printf("hi  = 0x%08x\n", glob->hi);
  printf("lo  = 0x%08x\n", glob->lo);
  for (int i = 1; i <= NB_REG; i++)
  {
    printf("r%-2d = 0x%08x ", i-1, glob->reg[i-1]);
    if (i % 8 == 0)
      printf("\n");
  }
}

void print_memory(void)
{
  for (size_t i = 0; i < 100; i++)
  {
    if (glob->map[i].addr != 0x0)
      printf("0x%08x = 0x%08x ", glob->map[i].addr, glob->map[i].value);
  }
  printf("\n");
}

void print_code(void) {
  uint32_t save_pc = glob->pc;
  
  for (glob->pc = 0; glob->pc < glob->file_size; glob->pc += 4)
  {
    uint32_t *instru = ((uint32_t *)glob->memory) + (glob->pc / 4);
    log_instr(instru, __FILENAME__, __LINE__);
  }
  glob->pc = save_pc;
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

int parse_cmd(struct vec *line)
{
  if (!isatty(STDIN_FILENO))
    return 0;

  struct cstream *cs = cstream_readline_create();

  vec_init(line);
  enum error er = get_content(cs, line);

  cstream_free(cs);
  if (er != NO_ERROR)
    return 0;
  return 1;
}

int get_arg(char *data)
{
  int i = 0;
  while (data[i] != 0)
  {
    if (data[i] == ' ')
      return i;
    i++;
  }
  return -1;
}

void help_debug(void)
{
  fprintf(stderr, "--------------------\n");
  fprintf(stderr, "registers           | print registers\n");
  fprintf(stderr, "memory              | print memory\n");
  fprintf(stderr, "code                | print code\n");
  fprintf(stderr, "print <pc_address>  | print instruction at specified pc_address\n");
  fprintf(stderr, "break <instruction> | set breakpoint\n");
  fprintf(stderr, "continue            | run until breakpoint or end\n");
  fprintf(stderr, "exit                | exit program\n");
  fprintf(stderr, "step                | execute next instruction\n");
  fprintf(stderr, "start               | set breakpoint at first instruction and start program\n");
  fprintf(stderr, "--------------------\n");
}

int start_debug(void)
{
  if (glob->pc != 0)
  {
    fprintf(stderr, "Do you want to restart the program ? [y/N] ");
    int r = getchar();
    if (r != 'y' && r != 'Y')
      return 1;
  }
  glob->pc = 0;
  uint32_t *instru = ((uint32_t *)glob->memory) + (glob->pc / 4);
  int ret = exec_inst(instru);
  if (ret)
  {
    if (ret == 1)
      fprintf(stderr, "Error during execution\n");
    return 0;
  }
  return 1;
}
void print_instruction(struct vec *buf)
{
  char *arg = calloc(100, 1);
  int index = get_arg(buf->data);
  strncpy(arg, buf->data + index, strlen(buf->data) - index);
  int e = 0;
  unsigned long addr = strtou32(arg, NULL, 16, &e);
  if (e)
    fprintf(stderr, "Bad address\n");
  else {
    uint32_t addr2 = *((uint32_t *)glob->memory) + addr;
    log_instr(&addr2, __FILENAME__, __LINE__);
  }
  free(arg);
}

uint32_t add_breakpoint(struct vec *buf)
{
  char *arg = calloc(100, 1);
  int index = get_arg(buf->data);
  strncpy(arg, buf->data + index, strlen(buf->data) - index);
  int e = 0;
  uint32_t addr = strtou32(arg, NULL, 16, &e);
  free(arg);
  if (e)
  {
    fprintf(stderr, "Bad address\n");
    return 0;
  }
  printf("Add breakpoint to: 0x%08x\n", addr);
  return addr;
}

int is_breakpoint(uint32_t value, uint32_t *break_list, size_t size)
{
  size_t i = 0;
  while (i <= size)
  {
    if (value == break_list[i])
      return 1;
    i++;
  }
  return 0;
}

int continue_exec(uint32_t *break_list, size_t size)
{
  while (1)
  {
    uint32_t *instru = ((uint32_t *)glob->memory) + (glob->pc / 4);
    int ret = exec_inst(instru);

    if (ret)
    {
      if (ret == 1)
        fprintf(stderr, "Error during execution\n");
      return 0;
    }
    if (is_breakpoint(glob->pc, break_list, size))
      break;

  }
  return 1;
}

void debug(void)
{
  struct vec *buf = calloc(1, sizeof(struct vec));
  uint32_t break_list[1000] = { -1 };
  size_t b_list_size = 0;
  while (1)
  {
    if (!parse_cmd(buf))
    {
      vec_destroy(buf);
      break;
    }

    if (!buf || !buf->data)
    {
      fprintf(stderr, "Error during parsing\n");
      vec_destroy(buf);
      break;
    }
    if (strlen(buf->data) == 0)
    {
      vec_destroy(buf);
      break;
    }
    if (!strcmp(buf->data, "registers"))
    {
      print_registers();
    }
    if (!strcmp(buf->data, "memory"))
    {
      print_memory();
    }
    if (!strcmp(buf->data, "code"))
    {
      print_code();
    }
    else if (!strcmp(buf->data, "start"))
    {
      if (!start_debug())
      {
        vec_destroy(buf);
        break;
      }
    }
    else if (!strcmp(buf->data, "step"))
    {
      uint32_t *instru = ((uint32_t *)glob->memory) + (glob->pc / 4);
      int ret = exec_inst(instru);
      if (ret)
      {
        if (ret == 1)
          fprintf(stderr, "Error during execution\n");
        vec_destroy(buf);
        break;
      }
    }
    else if (!strcmp(buf->data, "exit"))
    {
      vec_destroy(buf);
      break;
    }
    else if (!strncmp(buf->data, "print", 5))
    {
      print_instruction(buf);
    }
    else if (!strncmp(buf->data, "break", 5))
    {
      uint32_t addr = add_breakpoint(buf);
      if (addr)
        break_list[b_list_size] = addr;
    }
    else if (!strcmp(buf->data, "continue"))
    {
      if (!continue_exec(break_list, b_list_size))
      {
        vec_destroy(buf);
        break;
      }
    }
    else if (!strcmp(buf->data, "help"))
    {
      help_debug();
    }
    vec_destroy(buf);
  }
  free(buf);
  return;
}
