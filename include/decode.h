#ifndef DECODE_H
#define DECODE_H

#include <stdint.h>

/* ------------------------------------------------------------------ *
 *  MIPS instruction field accessors                                  *
 *                                                                    *
 *  R: | op(6) | rs(5) | rt(5) | rd(5) | shamt(5) | funct(6) |        *
 *  I: | op(6) | rs(5) | rt(5) |        imm(16)              |        *
 *  J: | op(6) |              target(26)                     |        *
 * ------------------------------------------------------------------ */

static inline uint8_t op(uint32_t i)
{
  return i >> 26;
}

static inline uint8_t rs(uint32_t i)
{
  return (i >> 21) & 0x1F;
}

static inline uint8_t rt(uint32_t i)
{
  return (i >> 16) & 0x1F;
}

static inline uint8_t rd(uint32_t i)
{
  return (i >> 11) & 0x1F;
}

static inline uint8_t shamt(uint32_t i)
{
  return (i >> 6) & 0x1F;
}

static inline uint8_t funct(uint32_t i)
{
  return i & 0x3F;
}

static inline uint16_t imm16(uint32_t i)
{
  return i & 0xFFFF;
}

static inline uint32_t target26(uint32_t i)
{
  return i & 0x3FFFFFF;
}

/* ------------------------------------------------------------------ *
 *  Opcodes (bits 31..26)                                             *
 * ------------------------------------------------------------------ */
#define OP_SPECIAL 0x00 /* R-type: dispatch on funct        */
#define OP_REGIMM 0x01  /* bltz/bgez/bltzal/bgezal on rt    */
#define OP_COP1 0x11    /* floating-point coprocessor       */
#define OP_SPECIAL2 0x1c /* MIPS32 mul/madd/clz on funct    */

/* COP1 floating-point load/store opcodes */
#define LWC1 0x31
#define LDC1 0x35
#define SWC1 0x39
#define SDC1 0x3d

/* COP1 format field (bits 25..21) */
#define FMT_MFC1 0x00
#define FMT_MTC1 0x04
#define FMT_BC1 0x08
#define FMT_S 0x10 /* single precision */
#define FMT_D 0x11 /* double precision */
#define FMT_W 0x14 /* fixed-point word  */

#define J 0x2
#define JAL 0x3
#define BEQ 0x4
#define BNE 0x5
#define BLEZ 0x6
#define BGTZ 0x7
#define ADDI 0x8
#define ADDIU 0x9
#define SLTI 0xa
#define SLTIU 0xb
#define ANDI 0xc
#define ORI 0xd
#define XORI 0xe
#define LUI 0xf
#define LB 0x20
#define LH 0x21
#define LWL 0x22
#define LW 0x23
#define LBU 0x24
#define LHU 0x25
#define LWR 0x26
#define SB 0x28
#define SH 0x29
#define SWL 0x2a
#define SW 0x2b
#define SWR 0x2e

/* rt field selector for the REGIMM (op 0x01) branches */
#define BLTZ 0x0
#define BGEZ 0x1
#define BLTZAL 0x10
#define BGEZAL 0x11

/* ------------------------------------------------------------------ *
 *  SPECIAL (op 0x00) function codes (bits 5..0)                      *
 * ------------------------------------------------------------------ */
#define SLL 0x0
#define SRL 0x2
#define SRA 0x3
#define SLLV 0x4
#define SRLV 0x6
#define SRAV 0x7
#define JR 0x8
#define JALR 0x9
#define MOVZ 0xa
#define MOVN 0xb
#define SYSCALL 0xc
#define BREAK 0xd
#define MFHI 0x10
#define MTHI 0x11
#define MFLO 0x12
#define MTLO 0x13
#define MULT 0x18
#define MULTU 0x19
#define DIV 0x1a
#define DIVU 0x1b
#define ADD 0x20
#define ADDU 0x21
#define SUB 0x22
#define SUBU 0x23
#define AND 0x24
#define OR 0x25
#define XOR 0x26
#define NOR 0x27
#define SLT 0x2a
#define SLTU 0x2b

/* ------------------------------------------------------------------ *
 *  SPECIAL2 (op 0x1c) function codes (MIPS32)                        *
 * ------------------------------------------------------------------ */
#define MADD 0x0
#define MADDU 0x1
#define MUL 0x2
#define MSUB 0x4
#define MSUBU 0x5
#define CLZ 0x20
#define CLO 0x21

#endif /* DECODE_H */
