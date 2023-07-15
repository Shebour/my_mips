#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <stdint.h>

#include "define.h"

struct global
{
  uint32_t reg[NB_REG];
  uint32_t hi;
  uint32_t lo;
  void *memory;
  uint32_t pc;
  size_t code_size;
  int debug;
};

int init_memory(char *path);
int init_global(int argc, char **argv);
void clean_exit(void);

#endif /* UTILS_H */
