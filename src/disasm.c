#include "disasm.h"

#include <stdio.h>

#include "decode.h"

/* R-format: rd, rs, rt   (e.g. "add r1, r2, r3") */
static void r3(char *o, size_t n, const char *m, uint32_t i)
{
  snprintf(o, n, "%s r%d, r%d, r%d", m, rd(i), rs(i), rt(i));
}

/* Shift by a constant amount: rd, rt, shamt */
static void rshift(char *o, size_t n, const char *m, uint32_t i)
{
  snprintf(o, n, "%s r%d, r%d, %d", m, rd(i), rt(i), shamt(i));
}

/* Variable shift: rd, rt, rs */
static void rshiftv(char *o, size_t n, const char *m, uint32_t i)
{
  snprintf(o, n, "%s r%d, r%d, r%d", m, rd(i), rt(i), rs(i));
}

static void disasm_special(uint32_t i, char *o, size_t n)
{
  switch (funct(i))
  {
  case ADD: r3(o, n, "add", i); break;
  case ADDU: r3(o, n, "addu", i); break;
  case SUB: r3(o, n, "sub", i); break;
  case SUBU: r3(o, n, "subu", i); break;
  case AND: r3(o, n, "and", i); break;
  case OR: r3(o, n, "or", i); break;
  case XOR: r3(o, n, "xor", i); break;
  case NOR: r3(o, n, "nor", i); break;
  case SLT: r3(o, n, "slt", i); break;
  case SLTU: r3(o, n, "sltu", i); break;
  case MOVZ: r3(o, n, "movz", i); break;
  case MOVN: r3(o, n, "movn", i); break;
  case SLL: rshift(o, n, "sll", i); break;
  case SRL: rshift(o, n, "srl", i); break;
  case SRA: rshift(o, n, "sra", i); break;
  case SLLV: rshiftv(o, n, "sllv", i); break;
  case SRLV: rshiftv(o, n, "srlv", i); break;
  case SRAV: rshiftv(o, n, "srav", i); break;
  case MULT: snprintf(o, n, "mult r%d, r%d", rs(i), rt(i)); break;
  case MULTU: snprintf(o, n, "multu r%d, r%d", rs(i), rt(i)); break;
  case DIV: snprintf(o, n, "div r%d, r%d", rs(i), rt(i)); break;
  case DIVU: snprintf(o, n, "divu r%d, r%d", rs(i), rt(i)); break;
  case MFHI: snprintf(o, n, "mfhi r%d", rd(i)); break;
  case MFLO: snprintf(o, n, "mflo r%d", rd(i)); break;
  case MTHI: snprintf(o, n, "mthi r%d", rs(i)); break;
  case MTLO: snprintf(o, n, "mtlo r%d", rs(i)); break;
  case JR: snprintf(o, n, "jr r%d", rs(i)); break;
  case JALR: snprintf(o, n, "jalr r%d, r%d", rd(i), rs(i)); break;
  case SYSCALL: snprintf(o, n, "syscall"); break;
  case BREAK: snprintf(o, n, "break"); break;
  default: snprintf(o, n, "unknown special 0x%x", funct(i)); break;
  }
}

static void disasm_special2(uint32_t i, char *o, size_t n)
{
  switch (funct(i))
  {
  case MUL: r3(o, n, "mul", i); break;
  case MADD: snprintf(o, n, "madd r%d, r%d", rs(i), rt(i)); break;
  case MADDU: snprintf(o, n, "maddu r%d, r%d", rs(i), rt(i)); break;
  case MSUB: snprintf(o, n, "msub r%d, r%d", rs(i), rt(i)); break;
  case MSUBU: snprintf(o, n, "msubu r%d, r%d", rs(i), rt(i)); break;
  case CLZ: snprintf(o, n, "clz r%d, r%d", rd(i), rs(i)); break;
  case CLO: snprintf(o, n, "clo r%d, r%d", rd(i), rs(i)); break;
  default: snprintf(o, n, "unknown special2 0x%x", funct(i)); break;
  }
}

static void disasm_regimm(uint32_t i, char *o, size_t n)
{
  switch (rt(i))
  {
  case BLTZ: snprintf(o, n, "bltz r%d, %d", rs(i), (int16_t)imm16(i)); break;
  case BGEZ: snprintf(o, n, "bgez r%d, %d", rs(i), (int16_t)imm16(i)); break;
  case BLTZAL:
    snprintf(o, n, "bltzal r%d, %d", rs(i), (int16_t)imm16(i));
    break;
  case BGEZAL:
    snprintf(o, n, "bgezal r%d, %d", rs(i), (int16_t)imm16(i));
    break;
  default: snprintf(o, n, "unknown regimm 0x%x", rt(i)); break;
  }
}

/* rt, rs, signed immediate */
static void i3s(char *o, size_t n, const char *m, uint32_t i)
{
  snprintf(o, n, "%s r%d, r%d, %d", m, rt(i), rs(i), (int16_t)imm16(i));
}

/* rt, rs, hex immediate (logical ops) */
static void i3x(char *o, size_t n, const char *m, uint32_t i)
{
  snprintf(o, n, "%s r%d, r%d, 0x%x", m, rt(i), rs(i), imm16(i));
}

/* load/store: rt, offset(rs) */
static void mem(char *o, size_t n, const char *m, uint32_t i)
{
  snprintf(o, n, "%s r%d, %d(r%d)", m, rt(i), (int16_t)imm16(i), rs(i));
}

static void disasm_immediate(uint32_t i, char *o, size_t n)
{
  switch (op(i))
  {
  case ADDI: i3s(o, n, "addi", i); break;
  case ADDIU: i3s(o, n, "addiu", i); break;
  case SLTI: i3s(o, n, "slti", i); break;
  case SLTIU: i3s(o, n, "sltiu", i); break;
  case ANDI: i3x(o, n, "andi", i); break;
  case ORI: i3x(o, n, "ori", i); break;
  case XORI: i3x(o, n, "xori", i); break;
  case LUI: snprintf(o, n, "lui r%d, 0x%x", rt(i), imm16(i)); break;
  case BEQ:
    snprintf(o, n, "beq r%d, r%d, %d", rs(i), rt(i), (int16_t)imm16(i));
    break;
  case BNE:
    snprintf(o, n, "bne r%d, r%d, %d", rs(i), rt(i), (int16_t)imm16(i));
    break;
  case BGTZ: snprintf(o, n, "bgtz r%d, %d", rs(i), (int16_t)imm16(i)); break;
  case BLEZ: snprintf(o, n, "blez r%d, %d", rs(i), (int16_t)imm16(i)); break;
  case LB: mem(o, n, "lb", i); break;
  case LBU: mem(o, n, "lbu", i); break;
  case LH: mem(o, n, "lh", i); break;
  case LHU: mem(o, n, "lhu", i); break;
  case LW: mem(o, n, "lw", i); break;
  case LWL: mem(o, n, "lwl", i); break;
  case LWR: mem(o, n, "lwr", i); break;
  case SB: mem(o, n, "sb", i); break;
  case SH: mem(o, n, "sh", i); break;
  case SW: mem(o, n, "sw", i); break;
  case SWL: mem(o, n, "swl", i); break;
  case SWR: mem(o, n, "swr", i); break;
  default: snprintf(o, n, "unknown opcode 0x%x", op(i)); break;
  }
}

static const char *fmt_suffix(uint8_t fmt)
{
  switch (fmt)
  {
  case FMT_S: return "s";
  case FMT_D: return "d";
  case FMT_W: return "w";
  default: return "?";
  }
}

static void disasm_cop1(uint32_t i, char *o, size_t n)
{
  uint8_t fmt = rs(i);
  if (fmt == FMT_MFC1)
  {
    snprintf(o, n, "mfc1 r%d, f%d", rt(i), rd(i));
    return;
  }
  if (fmt == FMT_MTC1)
  {
    snprintf(o, n, "mtc1 r%d, f%d", rt(i), rd(i));
    return;
  }
  if (fmt == FMT_BC1)
  {
    snprintf(o, n, "bc1%c %d", ((i >> 16) & 1) ? 't' : 'f', (int16_t)imm16(i));
    return;
  }
  const char *s = fmt_suffix(fmt);
  uint8_t fd = shamt(i), fs = rd(i), ft = rt(i);
  switch (funct(i))
  {
  case 0x00: snprintf(o, n, "add.%s f%d, f%d, f%d", s, fd, fs, ft); break;
  case 0x01: snprintf(o, n, "sub.%s f%d, f%d, f%d", s, fd, fs, ft); break;
  case 0x02: snprintf(o, n, "mul.%s f%d, f%d, f%d", s, fd, fs, ft); break;
  case 0x03: snprintf(o, n, "div.%s f%d, f%d, f%d", s, fd, fs, ft); break;
  case 0x04: snprintf(o, n, "sqrt.%s f%d, f%d", s, fd, fs); break;
  case 0x05: snprintf(o, n, "abs.%s f%d, f%d", s, fd, fs); break;
  case 0x06: snprintf(o, n, "mov.%s f%d, f%d", s, fd, fs); break;
  case 0x07: snprintf(o, n, "neg.%s f%d, f%d", s, fd, fs); break;
  case 0x20: snprintf(o, n, "cvt.s.%s f%d, f%d", s, fd, fs); break;
  case 0x21: snprintf(o, n, "cvt.d.%s f%d, f%d", s, fd, fs); break;
  case 0x24: snprintf(o, n, "cvt.w.%s f%d, f%d", s, fd, fs); break;
  case 0x32: snprintf(o, n, "c.eq.%s f%d, f%d", s, fs, ft); break;
  case 0x3c: snprintf(o, n, "c.lt.%s f%d, f%d", s, fs, ft); break;
  case 0x3e: snprintf(o, n, "c.le.%s f%d, f%d", s, fs, ft); break;
  default: snprintf(o, n, "cop1 0x%x", funct(i)); break;
  }
}

/* COP1 load/store: ftN, offset(base) */
static void fmem(char *o, size_t n, const char *m, uint32_t i)
{
  snprintf(o, n, "%s f%d, %d(r%d)", m, rt(i), (int16_t)imm16(i), rs(i));
}

void disassemble(uint32_t instr, char *out, size_t n)
{
  if (instr == 0)
  {
    snprintf(out, n, "nop");
    return;
  }
  switch (op(instr))
  {
  case OP_SPECIAL: disasm_special(instr, out, n); break;
  case OP_SPECIAL2: disasm_special2(instr, out, n); break;
  case OP_REGIMM: disasm_regimm(instr, out, n); break;
  case OP_COP1: disasm_cop1(instr, out, n); break;
  case LWC1: fmem(out, n, "lwc1", instr); break;
  case LDC1: fmem(out, n, "ldc1", instr); break;
  case SWC1: fmem(out, n, "swc1", instr); break;
  case SDC1: fmem(out, n, "sdc1", instr); break;
  case J: snprintf(out, n, "j 0x%x", target26(instr) << 2); break;
  case JAL: snprintf(out, n, "jal 0x%x", target26(instr) << 2); break;
  default: disasm_immediate(instr, out, n); break;
  }
}
