#define _GNU_SOURCE
#include "jit.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>

#include "cpu.h"
#include "decode.h"
#include "utils.h"

extern struct global *glob;

/* ------------------------------------------------------------------ *
 *  Native code buffer and block cache                                *
 * ------------------------------------------------------------------ */

typedef void (*block_fn)(void);

static uint8_t *code_buf;
static size_t code_off;
static size_t code_cap;
static uint8_t *cur; /* current emit cursor inside code_buf */

#define CACHE_SIZE (1u << 16)
struct cache_entry
{
  uint32_t pc;
  block_fn fn;
  int valid;
};
static struct cache_entry cache[CACHE_SIZE];

static void emit_init(void)
{
  code_cap = 16 * 1024 * 1024;
  code_buf = mmap(NULL, code_cap, PROT_READ | PROT_WRITE | PROT_EXEC,
                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (code_buf == MAP_FAILED)
    code_buf = NULL; /* JIT disabled; dispatcher falls back to interpreter */
  code_off = 0;
}

static block_fn cache_lookup(uint32_t pc)
{
  struct cache_entry *e = &cache[(pc >> 2) & (CACHE_SIZE - 1)];
  return (e->valid && e->pc == pc) ? e->fn : NULL;
}

static void cache_insert(uint32_t pc, block_fn fn)
{
  struct cache_entry *e = &cache[(pc >> 2) & (CACHE_SIZE - 1)];
  e->pc = pc;
  e->fn = fn;
  e->valid = 1;
}

/* ------------------------------------------------------------------ *
 *  x86-64 emitter                                                    *
 *                                                                    *
 *  r15 holds &glob->reg[0] (== glob, reg is the first member), so a   *
 *  guest register N lives at [r15 + N*4]. Scratch uses eax/ecx/edx.   *
 * ------------------------------------------------------------------ */

#define EAX 0
#define ECX 1
#define EDX 2

static void e8(uint8_t b)
{
  *cur++ = b;
}

static void e32(uint32_t v)
{
  e8(v);
  e8(v >> 8);
  e8(v >> 16);
  e8(v >> 24);
}

static void e64(uint64_t v)
{
  e32((uint32_t)v);
  e32((uint32_t)(v >> 32));
}

/* mov <x86reg>, [r15 + m*4] */
static void ld(int x86reg, uint8_t m)
{
  e8(0x41);
  e8(0x8B);
  e8(0x40 | (x86reg << 3) | 0x07);
  e8(m * 4);
}

/* mov [r15 + m*4], eax  (writes to $0 are discarded) */
static void st(uint8_t m)
{
  if (m == 0)
    return;
  e8(0x41);
  e8(0x89);
  e8(0x40 | 0x07);
  e8(m * 4);
}

/* mov eax, [r15 + off]  (off needs disp32: used for hi/lo) */
static void ld_off(uint32_t off)
{
  e8(0x41);
  e8(0x8B);
  e8(0x80 | 0x07);
  e32(off);
}

/* mov [r15 + off], eax */
static void st_off(uint32_t off)
{
  e8(0x41);
  e8(0x89);
  e8(0x80 | 0x07);
  e32(off);
}

/* eax <op> [r15 + rt*4] for a register-register ALU op */
static void alu_rr(uint8_t opcode, uint8_t rt)
{
  e8(0x41);
  e8(opcode);
  e8(0x47);
  e8(rt * 4);
}

/* eax = (eax <cc> 0/1) using setcc + movzx */
static void set_flag(uint8_t setcc)
{
  e8(0x0F);
  e8(setcc);
  e8(0xC0); /* setcc al */
  e8(0x0F);
  e8(0xB6);
  e8(0xC0); /* movzx eax, al */
}

/* ------------------------------------------------------------------ *
 *  Per-instruction code generation                                   *
 *                                                                    *
 *  Returns 1 if the instruction was compiled inline, 0 if it must be *
 *  left to the interpreter (which ends the current block).           *
 * ------------------------------------------------------------------ */

static int jit_emit(uint32_t i)
{
  switch (op(i))
  {
  case OP_SPECIAL:
    switch (funct(i))
    {
    case ADDU: ld(EAX, rs(i)); alu_rr(0x03, rt(i)); st(rd(i)); return 1;
    case SUBU: ld(EAX, rs(i)); alu_rr(0x2B, rt(i)); st(rd(i)); return 1;
    case AND: ld(EAX, rs(i)); alu_rr(0x23, rt(i)); st(rd(i)); return 1;
    case OR: ld(EAX, rs(i)); alu_rr(0x0B, rt(i)); st(rd(i)); return 1;
    case XOR: ld(EAX, rs(i)); alu_rr(0x33, rt(i)); st(rd(i)); return 1;
    case NOR:
      ld(EAX, rs(i));
      alu_rr(0x0B, rt(i)); /* or */
      e8(0xF7);
      e8(0xD0); /* not eax */
      st(rd(i));
      return 1;
    case SLT:
      ld(EAX, rs(i));
      alu_rr(0x3B, rt(i)); /* cmp */
      set_flag(0x9C);      /* setl */
      st(rd(i));
      return 1;
    case SLTU:
      ld(EAX, rs(i));
      alu_rr(0x3B, rt(i));
      set_flag(0x92); /* setb */
      st(rd(i));
      return 1;
    case SLL:
      ld(EAX, rt(i));
      e8(0xC1);
      e8(0xE0);
      e8(shamt(i));
      st(rd(i));
      return 1;
    case SRL:
      ld(EAX, rt(i));
      e8(0xC1);
      e8(0xE8);
      e8(shamt(i));
      st(rd(i));
      return 1;
    case SRA:
      ld(EAX, rt(i));
      e8(0xC1);
      e8(0xF8);
      e8(shamt(i));
      st(rd(i));
      return 1;
    case SLLV:
      ld(EAX, rt(i));
      ld(ECX, rs(i));
      e8(0xD3);
      e8(0xE0);
      st(rd(i));
      return 1;
    case SRLV:
      ld(EAX, rt(i));
      ld(ECX, rs(i));
      e8(0xD3);
      e8(0xE8);
      st(rd(i));
      return 1;
    case SRAV:
      ld(EAX, rt(i));
      ld(ECX, rs(i));
      e8(0xD3);
      e8(0xF8);
      st(rd(i));
      return 1;
    case MFHI: ld_off(offsetof(struct global, hi)); st(rd(i)); return 1;
    case MFLO: ld_off(offsetof(struct global, lo)); st(rd(i)); return 1;
    case MTHI: ld(EAX, rs(i)); st_off(offsetof(struct global, hi)); return 1;
    case MTLO: ld(EAX, rs(i)); st_off(offsetof(struct global, lo)); return 1;
    case MOVZ:
    case MOVN:
      ld(EAX, rd(i)); /* current value, kept if the move is skipped */
      ld(EDX, rs(i));
      ld(ECX, rt(i));
      e8(0x85);
      e8(0xC9); /* test ecx, ecx */
      e8(0x0F);
      e8(funct(i) == MOVZ ? 0x44 : 0x45); /* cmove / cmovne */
      e8(0xC2);                           /* eax, edx */
      st(rd(i));
      return 1;
    default: return 0;
    }
  case ADDIU:
    ld(EAX, rs(i));
    e8(0x05);
    e32((uint32_t)(int32_t)(int16_t)imm16(i)); /* add eax, sext(imm) */
    st(rt(i));
    return 1;
  case ANDI:
    ld(EAX, rs(i));
    e8(0x25);
    e32(imm16(i));
    st(rt(i));
    return 1;
  case ORI:
    ld(EAX, rs(i));
    e8(0x0D);
    e32(imm16(i));
    st(rt(i));
    return 1;
  case XORI:
    ld(EAX, rs(i));
    e8(0x35);
    e32(imm16(i));
    st(rt(i));
    return 1;
  case SLTI:
    ld(EAX, rs(i));
    e8(0x3D);
    e32((uint32_t)(int32_t)(int16_t)imm16(i)); /* cmp eax, sext(imm) */
    set_flag(0x9C);
    st(rt(i));
    return 1;
  case SLTIU:
    ld(EAX, rs(i));
    e8(0x3D);
    e32((uint32_t)(int32_t)(int16_t)imm16(i));
    set_flag(0x92);
    st(rt(i));
    return 1;
  case LUI:
    if (rt(i))
    {
      e8(0x41);
      e8(0xC7);
      e8(0x47);
      e8(rt(i) * 4); /* mov dword [r15+rt*4], imm */
      e32((uint32_t)imm16(i) << 16);
    }
    return 1;
  default: return 0;
  }
}

/* Compile the run of inline-able instructions starting at start_pc. Returns
 * NULL if the first instruction is not inline-able (the caller then steps the
 * interpreter for it). */
static block_fn compile_block(uint32_t start_pc)
{
  if (!code_buf || code_off + 8192 > code_cap)
    return NULL;

  uint8_t *block_start = code_buf + code_off;
  cur = block_start;

  e8(0x41);
  e8(0x57); /* push r15 */
  e8(0x49);
  e8(0xBF);
  e64((uint64_t)(uintptr_t)glob); /* movabs r15, glob */

  uint32_t pc = start_pc;
  int count = 0;
  while (count < 1024)
  {
    uint32_t instr = ((uint32_t *)glob->memory)[pc / 4];
    if (!jit_emit(instr))
      break;
    pc += 4;
    count++;
  }

  if (count == 0)
  {
    cur = block_start; /* nothing to do here; rewind */
    return NULL;
  }

  e8(0x48);
  e8(0xB8);
  e64((uint64_t)(uintptr_t)&glob->pc); /* movabs rax, &glob->pc */
  e8(0xC7);
  e8(0x00);
  e32(pc); /* mov dword [rax], pc */
  e8(0x41);
  e8(0x5F); /* pop r15 */
  e8(0xC3); /* ret */

  code_off = (size_t)(cur - code_buf);

  /* Object->function pointer cast via memcpy to stay -pedantic clean. */
  block_fn fn;
  memcpy(&fn, &block_start, sizeof(fn));
  return fn;
}

int execute_jit(void)
{
  if (!code_buf)
    emit_init();

  while (1)
  {
    /* In a delay slot: let the interpreter run it and apply the branch. */
    if (branch_is_pending())
    {
      int ret = step_instruction();
      if (ret == 1)
        return 1;
      if (ret == 2)
        return 0;
      continue;
    }

    uint32_t pc = glob->pc;
    block_fn fn = cache_lookup(pc);
    if (!fn)
      fn = compile_block(pc);

    if (fn)
    {
      cache_insert(pc, fn);
      fn(); /* runs native code, leaves glob->pc at the next instruction */
    }
    else
    {
      int ret = step_instruction(); /* control / memory / trapping op */
      if (ret == 1)
        return 1;
      if (ret == 2)
        return 0;
    }
  }
}
