#include "fpu.h"

#include <math.h>
#include <string.h>

#include "cpu.h"
#include "decode.h"
#include "functions.h"
#include "utils.h"

extern struct global *glob;

/* Bit-accurate access to the FP register file. Single precision uses one
 * 32-bit register; double precision uses an even/odd pair (FR=0 mode), with
 * the even register holding the low word. */

static float getf(uint8_t n)
{
  float f;
  memcpy(&f, &glob->fpr[n], sizeof(f));
  return f;
}

static void setf(uint8_t n, float f)
{
  memcpy(&glob->fpr[n], &f, sizeof(f));
}

static int32_t getw(uint8_t n)
{
  return (int32_t)glob->fpr[n];
}

static void setw(uint8_t n, int32_t w)
{
  glob->fpr[n] = (uint32_t)w;
}

static double getd(uint8_t n)
{
  uint64_t bits = (uint64_t)glob->fpr[n] | ((uint64_t)glob->fpr[n + 1] << 32);
  double d;
  memcpy(&d, &bits, sizeof(d));
  return d;
}

static void setd(uint8_t n, double d)
{
  uint64_t bits;
  memcpy(&bits, &d, sizeof(bits));
  glob->fpr[n] = (uint32_t)bits;
  glob->fpr[n + 1] = (uint32_t)(bits >> 32);
}

/* fmt = S: operate on single-precision floats. fields fd/fs/ft. */
static int fp_single(uint32_t i)
{
  uint8_t fd = shamt(i), fs = rd(i), ft = rt(i);
  switch (funct(i))
  {
  case 0x00: setf(fd, getf(fs) + getf(ft)); break; /* add.s */
  case 0x01: setf(fd, getf(fs) - getf(ft)); break; /* sub.s */
  case 0x02: setf(fd, getf(fs) * getf(ft)); break; /* mul.s */
  case 0x03: setf(fd, getf(fs) / getf(ft)); break; /* div.s */
  case 0x04: setf(fd, sqrtf(getf(fs))); break;     /* sqrt.s */
  case 0x05: setf(fd, fabsf(getf(fs))); break;     /* abs.s */
  case 0x06: setf(fd, getf(fs)); break;            /* mov.s */
  case 0x07: setf(fd, -getf(fs)); break;           /* neg.s */
  case 0x21: setd(fd, (double)getf(fs)); break;    /* cvt.d.s */
  case 0x24: setw(fd, (int32_t)getf(fs)); break;   /* cvt.w.s */
  case 0x32: glob->fp_cond = getf(fs) == getf(ft); break; /* c.eq.s */
  case 0x3c: glob->fp_cond = getf(fs) < getf(ft); break;  /* c.lt.s */
  case 0x3e: glob->fp_cond = getf(fs) <= getf(ft); break; /* c.le.s */
  default: return 1;
  }
  return 0;
}

/* fmt = D: operate on double-precision floats. */
static int fp_double(uint32_t i)
{
  uint8_t fd = shamt(i), fs = rd(i), ft = rt(i);
  switch (funct(i))
  {
  case 0x00: setd(fd, getd(fs) + getd(ft)); break; /* add.d */
  case 0x01: setd(fd, getd(fs) - getd(ft)); break; /* sub.d */
  case 0x02: setd(fd, getd(fs) * getd(ft)); break; /* mul.d */
  case 0x03: setd(fd, getd(fs) / getd(ft)); break; /* div.d */
  case 0x04: setd(fd, sqrt(getd(fs))); break;      /* sqrt.d */
  case 0x05: setd(fd, fabs(getd(fs))); break;      /* abs.d */
  case 0x06: setd(fd, getd(fs)); break;            /* mov.d */
  case 0x07: setd(fd, -getd(fs)); break;           /* neg.d */
  case 0x20: setf(fd, (float)getd(fs)); break;     /* cvt.s.d */
  case 0x24: setw(fd, (int32_t)getd(fs)); break;   /* cvt.w.d */
  case 0x32: glob->fp_cond = getd(fs) == getd(ft); break; /* c.eq.d */
  case 0x3c: glob->fp_cond = getd(fs) < getd(ft); break;  /* c.lt.d */
  case 0x3e: glob->fp_cond = getd(fs) <= getd(ft); break; /* c.le.d */
  default: return 1;
  }
  return 0;
}

/* fmt = W: convert a fixed-point word to floating point. */
static int fp_word(uint32_t i)
{
  uint8_t fd = shamt(i), fs = rd(i);
  switch (funct(i))
  {
  case 0x20: setf(fd, (float)getw(fs)); break;  /* cvt.s.w */
  case 0x21: setd(fd, (double)getw(fs)); break; /* cvt.d.w */
  default: return 1;
  }
  return 0;
}

int exec_cop1(uint32_t *instruction)
{
  uint32_t i = *instruction;
  switch (rs(i)) /* the format / sub-opcode field */
  {
  case FMT_MFC1: /* mfc1 rt, fs: GPR <- FPR bits */
    glob->reg[rt(i)] = glob->fpr[rd(i)];
    glob->reg[R0] = 0;
    return 0;
  case FMT_MTC1: /* mtc1 rt, fs: FPR bits <- GPR */
    glob->fpr[rd(i)] = glob->reg[rt(i)];
    return 0;
  case FMT_BC1: /* bc1f / bc1t: branch on the FP condition flag */
  {
    int tf = (i >> 16) & 1; /* 0 = bc1f, 1 = bc1t */
    if (glob->fp_cond == tf)
      schedule_branch(branch_dest(i));
    return 0;
  }
  case FMT_S: return fp_single(i);
  case FMT_D: return fp_double(i);
  case FMT_W: return fp_word(i);
  default: return 1;
  }
}

int exec_cop1_ldst(uint32_t *instruction)
{
  uint32_t i = *instruction;
  uint32_t addr = glob->reg[rs(i)] + (int16_t)imm16(i);
  uint8_t ft = rt(i);
  switch (op(i))
  {
  case LWC1: glob->fpr[ft] = load_word(addr); break;
  case SWC1: store_word(addr, glob->fpr[ft]); break;
  case LDC1:
    glob->fpr[ft] = load_word(addr);
    glob->fpr[ft + 1] = load_word(addr + 4);
    break;
  case SDC1:
    store_word(addr, glob->fpr[ft]);
    store_word(addr + 4, glob->fpr[ft + 1]);
    break;
  default: return 1;
  }
  return 0;
}
