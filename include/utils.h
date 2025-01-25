#ifndef UTILS_H
#define UTILS_H

#include <elf.h>
#include <stddef.h>
#include <stdint.h>

#include "define.h"

struct tuple {
  uint32_t addr;
  uint32_t value;
};

struct global
{
  uint32_t reg[NB_REG];
  uint32_t hi;
  uint32_t lo;
  void *memory;
  struct tuple map[100];
  size_t map_index;
  uint32_t pc;
  size_t file_size;
  size_t code_size;
  int debug;
  int log;
  int elf;
  void *elf_file;
  Elf32_Ehdr elf_header;
  Elf32_Phdr *prg_header;
};

int init_global(int argc, char **argv);
void clean_exit(void);
uint32_t strtou32(const char *nptr, char **endptr, int base, int *rstatus);

#endif /* UTILS_H */
