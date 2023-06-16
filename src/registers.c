#include "registers.h"

#include <stdio.h>

uint32_t registers[NB_REG];

void init_reg()
{
  for(int i = 0; i < NB_REG; i++)
  {
    registers[i] = 0x0;
#ifdef DEBUG
    printf("registers[%d] = 0x0\n", i);
#endif
  }
}
