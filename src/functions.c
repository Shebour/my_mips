#include "functions.h"

#include <inttypes.h>
#include <stdio.h>

#include "utils.h"

extern struct global *glob;

void add(uint32_t rs, uint32_t rt, uint32_t rd)
{
  int32_t rs_val = glob->reg[rs];
  int32_t rt_val = glob->reg[rt];
  int32_t res = 0;
  if (__builtin_add_overflow(rs_val, rt_val, &res))
  {
    fprintf(stderr, "[my_mips] Overflow occurred!\n");
    clean_exit();
  }
  else
  {
    glob->reg[rd] = res;
  }
}
