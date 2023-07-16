#include "functions.h"

#include <inttypes.h>
#include <stdio.h>

#include "utils.h"

extern struct global *glob;

void trap(void)
{
  glob->reg[K0] = 1;
}

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

void addu(uint32_t rs, uint32_t rt, uint32_t rd)
{
  uint32_t rs_val = glob->reg[rs];
  uint32_t rt_val = glob->reg[rt];
  uint32_t res = rs_val + rt_val;
  glob->reg[rd] = res;
}

void sub(uint32_t rs, uint32_t rt, uint32_t rd)
{
  int32_t rs_val = glob->reg[rs];
  int32_t rt_val = glob->reg[rt];
  int32_t res = 0;
  if (__builtin_sub_overflow(rs_val, rt_val, &res))
  {
    fprintf(stderr, "[my_mips] Overflow occurred!\n");
    clean_exit();
  }
  else
  {
    glob->reg[rd] = res;
  }
}

void subu(uint32_t rs, uint32_t rt, uint32_t rd)
{
  uint32_t rs_val = glob->reg[rs];
  uint32_t rt_val = glob->reg[rt];
  uint32_t res = rs_val - rt_val;
  glob->reg[rd] = res;
}

void div(uint32_t rs, uint32_t rt)
{
  int32_t rs_val = glob->reg[rs];
  int32_t rt_val = glob->reg[rt];
  if (rt_val == 0)
    return;
  uint32_t q = rs_val / rt_val;
  uint32_t r = rs_val % rt_val;
  glob->hi = q;
  glob->lo = r;
}

void divu(uint32_t rs, uint32_t rt)
{
  uint32_t rs_val = glob->reg[rs];
  uint32_t rt_val = glob->reg[rt];
  if (rt_val == 0)
    return;
  uint32_t q = rs_val / rt_val;
  uint32_t r = rs_val % rt_val;
  glob->hi = q;
  glob->lo = r;
}

void mult(uint32_t rs, uint32_t rt)
{
  int32_t rs_val = glob->reg[rs];
  int32_t rt_val = glob->reg[rt];
  int64_t res = rs_val * rt_val;
  glob->hi = (int32_t)res;
  glob->lo = res >> 32;
}

void multu(uint32_t rs, uint32_t rt)
{
  uint32_t rs_val = glob->reg[rs];
  uint32_t rt_val = glob->reg[rt];
  uint64_t res = rs_val * rt_val;
  glob->hi = (uint32_t)res;
  glob->lo = res >> 32;
}

void slt(uint32_t rs, uint32_t rt, uint32_t rd)
{
  int32_t rs_val = glob->reg[rs];
  int32_t rt_val = glob->reg[rt];
  if (rs_val < rt_val)
    glob->reg[rd] = 1;
  else
    glob->reg[rd] = 0;
}

void sltu(uint32_t rs, uint32_t rt, uint32_t rd)
{
  uint32_t rs_val = glob->reg[rs];
  uint32_t rt_val = glob->reg[rt];
  if (rs_val < rt_val)
    glob->reg[rd] = 1;
  else
    glob->reg[rd] = 0;
}
