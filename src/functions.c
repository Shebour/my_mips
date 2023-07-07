#include "functions.h"

#include "utils.h"

extern struct global *glob;

int add(uint32_t rs, uint32_t rt, uint32_t rd)
{
   res = (int32_t) glob->reg[rs] + (int32_t) glob->reg[rt];
  if ((int32_t) glob->reg[rs] < 0 && (int32_t) glob->reg[rt] < 0 && res > 0)
    return 0;
  if ((int32_t) glob->reg[rs] > 0 && (int32_t) glob->reg[rt] > 0 && res < 0)
    return 0;
  glob->reg[rd] = glob->reg[rs] + glob->reg[rt];
  return 1;
}

int addu(uint32_t rs, uint32_t rt, uint32_t rd)
{
  uint32_t res = glob->reg[rs] + glob->reg[rt];

  return 1;
}
