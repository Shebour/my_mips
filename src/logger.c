#include "logger.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#include "disasm.h"
#include "utils.h"

extern struct global *glob;

#define MAX_LOC_LEN 20

static void align_location(const char *file, int line, char *str)
{
  char line_str[7] = { 0 };
  int len_line = snprintf(line_str, 7, "%d", line);
  size_t len_filename = strlen(file);
  int len_total = len_line + len_filename + 1;
  if (len_total > MAX_LOC_LEN)
  {
    int off = len_total - MAX_LOC_LEN + 3;
    snprintf(str, MAX_LOC_LEN, "...%s:%s", file + off, line_str);
  }
  else
    snprintf(str, MAX_LOC_LEN, "%s:%s", file, line_str);
}

void logger(const char *file, int line, const char *fmt, ...)
{
  char str[MAX_LOC_LEN] = { 0 };
  align_location(file, line, str);
  va_list args_list;
  va_start(args_list, fmt);
  fprintf(stderr, "[my_mips] ");
  vfprintf(stderr, fmt, args_list);
  fprintf(stderr, " (%s)", str);
  fprintf(stderr, "\n");
  fflush(stderr);
  va_end(args_list);
}

/* Trace one executed instruction (used by the -l flag). The mnemonic comes
 * from the shared disassembler so this never drifts from the decoder. */
void log_instr(uint32_t *instru, char *filename, int line)
{
  char dis[64];
  disassemble(*instru, dis, sizeof(dis));
  fprintf(stderr, "[my_mips] Executing pc = 0x%08x: 0x%08x: %s (%s:%d)\n",
          glob->pc, *instru, dis, filename, line);
}
