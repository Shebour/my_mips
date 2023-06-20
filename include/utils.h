#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <stdint.h>

#include "define.h"

struct global
{
  uint32_t reg[NB_REG];
  void *gp;
  void *sp;
  void *fp;
  void *ra;
  void *memory;
  size_t pc;
  size_t code_size;
};

int init_memory(char *path);
int init_global(char *path);

#endif /* UTILS_H */
