#include "instructions.h"

#include <stdio.h>

#include "syscalls.h"
#include "utils.h"

extern struct global *glob;

void parse_register(uint32_t *instruction)
{
  printf("%08x Register instruction\n", *instruction);
}

void parse_immediate(uint32_t *instruction)
{
  printf("%08x Immediate instruction\n", *instruction);
}

void parse_jump(uint32_t *instruction)
{
  printf("%08x Jump instruction\n", *instruction);
}

void parse_inst()
{
  for (size_t i = 0; i < (MEM_SIZE / sizeof(uint32_t)); i++)
  {
    uint32_t *instru = ((uint32_t *)glob->memory) + i;
    printf("%p : %08x\n", (uint8_t *)glob->memory + i * sizeof(uint32_t),
           *instru);
    if (*instru == 0 || *instru == 0xa)
      continue;
    if (*instru == 0xc)
    {
      if (call_syscall())
        return;
      continue;
    }
    uint8_t opcode = (*instru) >> 26;
    if (opcode == 0)
      parse_register(instru);
    else if (opcode == 2 || opcode == 3 || opcode == 26)
      parse_jump(instru);
    else
      parse_immediate(instru);
  }
}
