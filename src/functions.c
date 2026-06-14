#include "functions.h"

#include "decode.h"
#include "utils.h"

extern struct global *glob;

/* This module holds the instruction semantics that need more than a single
 * expression: signed arithmetic with overflow handling, the 64-bit
 * multiply/divide, and the byte-addressed memory accessors. The simple
 * one-expression operations are inlined directly in cpu.c's dispatch. */

/* Signed overflow writes 1 to $k0 (0 otherwise) per the subject instead of
 * raising a real trap; the destination register is left unchanged. */
void add(uint32_t i)
{
  int32_t res;
  if (__builtin_add_overflow((int32_t)glob->reg[rs(i)],
                             (int32_t)glob->reg[rt(i)], &res))
    glob->reg[K0] = 1;
  else
  {
    glob->reg[K0] = 0;
    glob->reg[rd(i)] = res;
  }
}

void sub(uint32_t i)
{
  int32_t res;
  if (__builtin_sub_overflow((int32_t)glob->reg[rs(i)],
                             (int32_t)glob->reg[rt(i)], &res))
    glob->reg[K0] = 1;
  else
  {
    glob->reg[K0] = 0;
    glob->reg[rd(i)] = res;
  }
}

void addi(uint32_t i)
{
  int32_t res;
  if (__builtin_add_overflow((int32_t)glob->reg[rs(i)], (int16_t)imm16(i), &res))
    glob->reg[K0] = 1;
  else
  {
    glob->reg[K0] = 0;
    glob->reg[rt(i)] = res;
  }
}

/* mult/div: quotient/low product -> LO, remainder/high product -> HI. */
void mult(uint32_t i)
{
  int64_t res = (int64_t)(int32_t)glob->reg[rs(i)] * (int32_t)glob->reg[rt(i)];
  glob->lo = (uint32_t)res;
  glob->hi = (uint32_t)(res >> 32);
}

void multu(uint32_t i)
{
  uint64_t res = (uint64_t)glob->reg[rs(i)] * glob->reg[rt(i)];
  glob->lo = (uint32_t)res;
  glob->hi = (uint32_t)(res >> 32);
}

void div(uint32_t i)
{
  int32_t a = glob->reg[rs(i)];
  int32_t b = glob->reg[rt(i)];
  if (b == 0)
    return; /* result is architecturally unpredictable */
  glob->lo = (uint32_t)(a / b);
  glob->hi = (uint32_t)(a % b);
}

void divu(uint32_t i)
{
  uint32_t a = glob->reg[rs(i)];
  uint32_t b = glob->reg[rt(i)];
  if (b == 0)
    return;
  glob->lo = a / b;
  glob->hi = a % b;
}

/* ---- byte-addressed little-endian memory ---- */

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
