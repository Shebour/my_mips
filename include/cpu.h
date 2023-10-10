#ifndef CPU_H
#define CPU_H

#include <stddef.h>
#include <stdint.h>

enum inst_type
{
  JUMP,
  IMM,
  REG
};

// Register encoding
#define ADD 0x20
#define ADDU 0x21
#define DIV 0x1a
#define DIVU 0x1b
#define MULT 0x18
#define MULTU 0x19
#define SUB 0x22
#define SUBU 0x23
#define AND 0x24
#define NOR 0x27
#define OR 0x25
#define SLT 0x2a
#define SLTU 0x29
#define XOR 0x26
#define SLL 0x0
#define SLLV 0x4
#define SRA 0x3
#define SRAV 0x7
#define SRL 0x2
#define SRLV 0x6
#define JALR 0x9
#define JR 0x8
#define MFHI 0x10
#define MFLO 0x12
#define MTHI 0x11
#define MTLO 0x13

// Immediate encoding
#define ADDI 0x8
#define ADDIU 0x9
#define ANDI 0xc
#define ORI 0xd
#define XORI 0xe
#define SLTI 0xa
#define SLTIU 0xb
#define BEQ 0x4
#define BNE 0x5
#define BGTZ 0x7
#define BLEZ 0x6
#define BGEZ 0x1
#define BGEZAL 0x11
#define BLTZ 0x0
#define BLTZAL 0x10
#define LB 0x20
#define LBU 0x24
#define LH 0x21
#define LHU 0x25
#define LW 0x23
#define LWL 0x22
#define LWR 0x26
#define SB 0x28
#define SH 0x29
#define SW 0x2b
#define SWL 0x2a
#define SWR 0x2e
#define LUI 0xf

// Jump encoding
#define J 0x2
#define JAL 0x3
#define TRAP 0x1a

int execute();

#endif /* CPU_H */
