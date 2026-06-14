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
  glob->reg[rd] = rs_val + rt_val;
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
  glob->lo = (uint32_t)(rs_val / rt_val); /* quotient  */
  glob->hi = (uint32_t)(rs_val % rt_val); /* remainder */
}

void divu(uint32_t rs, uint32_t rt)
{
  uint32_t rs_val = glob->reg[rs];
  uint32_t rt_val = glob->reg[rt];
  if (rt_val == 0)
    return;
  glob->lo = rs_val / rt_val; /* quotient  */
  glob->hi = rs_val % rt_val; /* remainder */
}

void mult(uint32_t rs, uint32_t rt)
{
  int32_t rs_val = glob->reg[rs];
  int32_t rt_val = glob->reg[rt];
  int64_t res = (int64_t)rs_val * rt_val;
  glob->lo = (uint32_t)res;
  glob->hi = (uint32_t)(res >> 32);
}

void multu(uint32_t rs, uint32_t rt)
{
  uint32_t rs_val = glob->reg[rs];
  uint32_t rt_val = glob->reg[rt];
  uint64_t res = (uint64_t)rs_val * rt_val;
  glob->lo = (uint32_t)res;
  glob->hi = (uint32_t)(res >> 32);
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

void addi(uint32_t rs, int32_t imm, uint32_t rt)
{
  int32_t rs_val = glob->reg[rs];
  int32_t res = 0;
  if (__builtin_add_overflow(rs_val, imm, &res))
  {
    fprintf(stderr, "[my_mips] Overflow occurred!\n");
    clean_exit();
  }
  else
  {
    glob->reg[rt] = res;
  }
}

void addiu(uint32_t rs, int32_t imm, uint32_t rt)
{
  uint32_t rs_val = glob->reg[rs];
  glob->reg[rt] = rs_val + imm;
}

void slti(uint32_t rs, int32_t imm, uint32_t rt)
{
  int32_t rs_val = glob->reg[rs];
  if (rs_val < imm)
    glob->reg[rt] = 1;
  else
    glob->reg[rt] = 0;
}

void sltiu(uint32_t rs, uint32_t imm, uint32_t rt)
{
  uint32_t rs_val = glob->reg[rs];
  if (rs_val < imm)
    glob->reg[rt] = 1;
  else
    glob->reg[rt] = 0;
}

uint32_t load_byte(uint32_t address)
{
  return ((uint8_t *)glob->memory)[address];
}

uint32_t load_half(uint32_t address)
{
  uint8_t *m = (uint8_t *)glob->memory;
  return m[address] | (m[address + 1] << 8);
}

uint32_t load_word(uint32_t address)
{
  uint8_t *m = (uint8_t *)glob->memory;
  return m[address] | (m[address + 1] << 8) | (m[address + 2] << 16)
      | (m[address + 3] << 24);
}

void store_byte(uint32_t address, uint32_t value)
{
  ((uint8_t *)glob->memory)[address] = value & 0xFF;
}

void store_half(uint32_t address, uint32_t value)
{
  uint8_t *m = (uint8_t *)glob->memory;
  m[address] = value & 0xFF;
  m[address + 1] = (value >> 8) & 0xFF;
}

void store_word(uint32_t address, uint32_t value)
{
  uint8_t *m = (uint8_t *)glob->memory;
  m[address] = value & 0xFF;
  m[address + 1] = (value >> 8) & 0xFF;
  m[address + 2] = (value >> 16) & 0xFF;
  m[address + 3] = (value >> 24) & 0xFF;
}
