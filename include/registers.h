#ifndef REGISTERS_H
#define REGISTERS_H

#include <stdint.h>

#include "define.h"

struct registers
{
  uint32_t reg[NB_REG];
  void *gp;
  void *sp;
  void *fp;
  void *ra;
};

struct registers *init_reg(void *sp);

#endif /* REGISTERS_H */
