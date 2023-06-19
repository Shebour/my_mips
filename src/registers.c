#include "registers.h"

#include <stddef.h>
#include <stdlib.h>

struct registers *init_reg(void *sp)
{
  struct registers *regs = malloc(sizeof(struct registers));
  if (!regs)
    return NULL;
  for (int i = 0; i < NB_REG; i++)
  {
    regs->reg[i] = 0x0;
  }
  regs->sp = sp;
  regs->gp = 0x0;
  regs->ra = 0x0;
  return regs;
}
