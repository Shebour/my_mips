#include "dbg.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cpu.h"
#include "cstream.h"
#include "disasm.h"
#include "error.h"
#include "utils.h"
#include "vec.h"

extern struct global *glob;

#define MAX_BREAKPOINTS 128

static uint32_t breakpoints[MAX_BREAKPOINTS];
static size_t nbreakpoints = 0;
static int halted = 0; /* set once the program exits or errors */

static uint32_t word_at(uint32_t addr)
{
  return ((uint32_t *)glob->memory)[addr / 4];
}

void print_registers(void)
{
  printf("pc = 0x%08x   hi = 0x%08x   lo = 0x%08x\n", glob->pc, glob->hi,
         glob->lo);
  for (int i = 0; i < NB_REG; i++)
  {
    printf("r%-2d = 0x%08x", i, glob->reg[i]);
    printf((i % 4 == 3) ? "\n" : "   ");
  }
}

/* Print "Executing pc = ...: <hex>: <disasm>" for the instruction at pc. */
static void trace_instr(uint32_t pc)
{
  char dis[64];
  uint32_t instr = word_at(pc);
  disassemble(instr, dis, sizeof(dis));
  printf("Executing pc = 0x%08x: 0x%08x: %s\n", pc, instr, dis);
}

static int bp_contains(uint32_t addr)
{
  for (size_t i = 0; i < nbreakpoints; i++)
    if (breakpoints[i] == addr)
      return 1;
  return 0;
}

static void bp_add(uint32_t addr)
{
  if (bp_contains(addr))
    return;
  if (nbreakpoints >= MAX_BREAKPOINTS)
  {
    printf("Too many breakpoints\n");
    return;
  }
  breakpoints[nbreakpoints++] = addr;
  printf("Breakpoint set at 0x%08x\n", addr);
}

static void bp_delete(uint32_t addr)
{
  for (size_t i = 0; i < nbreakpoints; i++)
    if (breakpoints[i] == addr)
    {
      breakpoints[i] = breakpoints[--nbreakpoints];
      printf("Breakpoint removed at 0x%08x\n", addr);
      return;
    }
  printf("No breakpoint at 0x%08x\n", addr);
}

static void bp_list(void)
{
  if (nbreakpoints == 0)
  {
    printf("No breakpoints\n");
    return;
  }
  for (size_t i = 0; i < nbreakpoints; i++)
    printf("  0x%08x\n", breakpoints[i]);
}

/* Run one instruction, reporting a clean exit or an error. */
static int do_step(void)
{
  if (halted)
  {
    printf("Program has finished.\n");
    return 0;
  }
  trace_instr(glob->pc);
  int ret = step_instruction();
  if (ret == 1)
  {
    printf("Error during execution\n");
    halted = 1;
  }
  else if (ret == 2)
  {
    printf("Program exited.\n");
    halted = 1;
  }
  return ret;
}

static void do_continue(void)
{
  if (halted)
  {
    printf("Program has finished.\n");
    return;
  }
  while (1)
  {
    if (do_step() != 0)
      return;
    if (bp_contains(glob->pc))
    {
      printf("Breakpoint at 0x%08x\n", glob->pc);
      return;
    }
  }
}

/* Disassemble the whole loaded program. */
static void print_code(void)
{
  char dis[64];
  for (uint32_t pc = 0; pc < glob->file_size; pc += 4)
  {
    uint32_t instr = word_at(pc);
    disassemble(instr, dis, sizeof(dis));
    printf("0x%08x: 0x%08x: %s\n", pc, instr, dis);
  }
}

static void print_word(uint32_t addr)
{
  char dis[64];
  uint32_t instr = word_at(addr);
  disassemble(instr, dis, sizeof(dis));
  printf("0x%08x: 0x%08x: %s\n", addr, instr, dis);
}

static void dump_memory(uint32_t addr, size_t count)
{
  for (size_t i = 0; i < count; i++)
    printf("0x%08x: 0x%08x\n", addr + (uint32_t)(i * 4), word_at(addr + i * 4));
}

static void help_debug(void)
{
  printf("Commands:\n");
  printf("  step [n]        execute n instructions (default 1)\n");
  printf("  continue        run until a breakpoint or the end\n");
  printf("  break <addr>    set a breakpoint (hex address)\n");
  printf("  delete <addr>   remove a breakpoint\n");
  printf("  breakpoints     list breakpoints\n");
  printf("  registers       print the registers\n");
  printf("  print <addr>    disassemble the instruction at addr\n");
  printf("  memory <addr> [n]  dump n memory words (default 1)\n");
  printf("  code            disassemble the whole program\n");
  printf("  help            show this help\n");
  printf("  exit            quit the debugger\n");
}

/* --- command parsing helpers --- */

/* True if line starts with command a (or alias b) at a word boundary. */
static int is_cmd(const char *line, const char *a, const char *b)
{
  size_t la = strlen(a);
  if (!strncmp(line, a, la) && (line[la] == 0 || line[la] == ' '))
    return 1;
  if (b)
  {
    size_t lb = strlen(b);
    if (!strncmp(line, b, lb) && (line[lb] == 0 || line[lb] == ' '))
      return 1;
  }
  return 0;
}

/* Pointer to the first argument after the command word, or NULL. */
static char *cmd_arg(char *line)
{
  while (*line && *line != ' ')
    line++;
  while (*line == ' ')
    line++;
  return *line ? line : NULL;
}

static int parse_addr(char *line, uint32_t *out)
{
  char *a = cmd_arg(line);
  if (!a)
  {
    printf("Missing address\n");
    return 0;
  }
  /* Copy just the first token so a trailing count (e.g. "memory 0x0 3")
   * does not confuse the hex parser. */
  char tok[32];
  size_t i = 0;
  while (a[i] && a[i] != ' ' && i < sizeof(tok) - 1)
  {
    tok[i] = a[i];
    i++;
  }
  tok[i] = 0;
  int status = 0;
  *out = strtou32(tok, NULL, 16, &status);
  if (status)
  {
    printf("Bad address\n");
    return 0;
  }
  return 1;
}

static void run_command(char *line, int *quit)
{
  uint32_t addr;
  if (is_cmd(line, "step", "s"))
  {
    char *a = cmd_arg(line);
    long n = a ? strtol(a, NULL, 10) : 1;
    for (long i = 0; i < n && !halted; i++)
      do_step();
  }
  else if (is_cmd(line, "continue", "c"))
    do_continue();
  else if (is_cmd(line, "break", "b"))
  {
    if (parse_addr(line, &addr))
      bp_add(addr);
  }
  else if (is_cmd(line, "delete", "d"))
  {
    if (parse_addr(line, &addr))
      bp_delete(addr);
  }
  else if (is_cmd(line, "breakpoints", NULL))
    bp_list();
  else if (is_cmd(line, "registers", "regs"))
    print_registers();
  else if (is_cmd(line, "print", "p"))
  {
    if (parse_addr(line, &addr))
      print_word(addr);
  }
  else if (is_cmd(line, "memory", "m"))
  {
    if (parse_addr(line, &addr))
    {
      /* optional second token after the address = word count */
      char *a = cmd_arg(line);
      char *sp = strchr(a, ' ');
      size_t count = sp ? (size_t)strtoul(sp + 1, NULL, 10) : 1;
      dump_memory(addr, count ? count : 1);
    }
  }
  else if (is_cmd(line, "code", NULL))
    print_code();
  else if (is_cmd(line, "help", "h"))
    help_debug();
  else if (is_cmd(line, "exit", "quit") || is_cmd(line, "q", NULL))
    *quit = 1;
  else
    printf("Unknown command: %s (try 'help')\n", line);
}

static enum error read_line(struct cstream *cs, struct vec *line)
{
  enum error err;
  while (1)
  {
    int c;
    if ((err = cstream_pop(cs, &c)))
      return err;
    if (c == EOF || c == '\n')
    {
      vec_push(line, 0);
      return NO_ERROR;
    }
    vec_push(line, c);
  }
}

void debug(void)
{
  int interactive = isatty(STDIN_FILENO);
  struct cstream *cs = interactive ? cstream_readline_create()
                                   : cstream_file_create(stdin, false);
  int quit = 0;
  while (!quit)
  {
    struct vec line;
    vec_init(&line);
    if (read_line(cs, &line) != NO_ERROR || line.size <= 1)
    {
      vec_destroy(&line);
      break; /* EOF or empty line */
    }
    run_command(line.data, &quit);
    vec_destroy(&line);
  }
  cstream_free(cs);
}
