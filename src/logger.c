#include "logger.h"

#include <stdint.h>
#include <stdio.h>

#include "utils.h"

extern struct global *glob;

void log_reg_instr(uint32_t *instru)
{
  uint32_t function = (*instru) << 26;
  function = function >> 26;
  uint32_t rs = ((*instru) >> 21) & 0x1F;
  uint32_t rt = ((*instru) >> 16) & 0x1F;
  uint32_t rd = ((*instru) >> 11) & 0x1F;
  uint32_t sa = ((*instru) >> 6) & 0x1F;
  fprintf(stderr, "[my_mips] Executing pc = %08x: %08x: ", glob->pc, *instru);
  switch (function)
  {
  case ADD:
    fprintf(stderr, "add r%d, r%d, r%d\n", rd, rs, rt);
    break;
  case ADDU:
    fprintf(stderr, "addu r%d, r%d, r%d\n", rd, rs, rt);
    break;
  case DIV:
    fprintf(stderr, "div r%d, r%d\n", rs, rt);
    break;
  case DIVU:
    fprintf(stderr, "divu r%d, r%d\n", rs, rt);
    break;
  case MULT:
    fprintf(stderr, "mult r%d, r%d, r%d\n", rd, rs, rt);
    break;
  case MULTU:
    fprintf(stderr, "multu r%d, r%d, r%d\n", rd, rs, rt);
    break;
  case SUB:
    fprintf(stderr, "sub r%d, r%d, r%d\n", rd, rs, rt);
    break;
  case SUBU:
    fprintf(stderr, "subu r%d, r%d, r%d\n", rd, rs, rt);
    break;
  case AND:
    fprintf(stderr, "and r%d, r%d, r%d\n", rd, rs, rt);
    break;
  case NOR:
    fprintf(stderr, "nor r%d, r%d, r%d\n", rd, rs, rt);
    break;
  case OR:
    fprintf(stderr, "or r%d, r%d, r%d\n", rd, rs, rt);
    break;
  case SLT:
    fprintf(stderr, "slt r%d, r%d, r%d\n", rd, rs, rt);
    break;
  case SLTU:
    fprintf(stderr, "sltu r%d, r%d, r%d\n", rd, rs, rt);
    break;
  case XOR:
    fprintf(stderr, "xor r%d, r%d, r%d\n", rd, rs, rt);
    break;
  case SLL:
    fprintf(stderr, "sll r%d, r%d, r%d\n", rd, rt, sa);
    break;
  case SLLV:
    fprintf(stderr, "sllv r%d, r%d, r%d\n", rd, rt, rs);
    break;
  case SRA:
    fprintf(stderr, "sra r%d, r%d, r%d\n", rd, rt, sa);
    break;
  case SRAV:
    fprintf(stderr, "srav r%d, r%d, r%d\n", rd, rt, rs);
    break;
  case SRL:
    fprintf(stderr, "srl r%d, r%d, r%d\n", rd, rt, sa);
    break;
  case SRLV:
    fprintf(stderr, "srlv r%d, r%d, r%d\n", rd, rt, rs);
    break;
  case JALR:
    fprintf(stderr, "jalr r%d, r%d\n", rd, rs);
    break;
  case JR:
    fprintf(stderr, "jr r%d\n", rs);
    break;
  case MFHI:
    fprintf(stderr, "mfhi r%d\n", rd);
    break;
  case MFLO:
    fprintf(stderr, "mflo r%d\n", rd);
    break;
  case MTHI:
    fprintf(stderr, "mthi r%d\n", rs);
    break;
  case MTLO:
    fprintf(stderr, "mtlo r%d\n", rs);
    break;
  default:
    perror("Bad register instruction\n");
    break;
  }
}

void log_imm_instr(uint32_t *instru)
{
  uint8_t opcode = (*instru) >> 26;
  uint32_t rs = ((*instru) >> 21) & 0x1F;
  uint32_t rt = ((*instru) >> 16) & 0x1F;
  uint32_t imm = (*instru) & 0xFFFF;
  fprintf(stderr, "[my_mips] Executing pc = %08x: %08x: ", glob->pc, *instru);
  if (opcode == 1)
  {
    switch (rt)
    {
    case BGEZ:
      fprintf(stderr, "bgez r%d, %x\n", rs, imm);
      break;
    case BGEZAL:
      fprintf(stderr, "bgezal r%d, %x\n", rs, imm);
      break;
    case BLTZ:
      fprintf(stderr, "bltz r%d, %x\n", rs, imm);
      break;
    case BLTZAL:
      fprintf(stderr, "bltz r%d, %x\n", rs, imm);
      break;
    }
  }
  switch (opcode)
  {
  case ADDI:
    fprintf(stderr, "addi r%d, r%d, %x\n", rt, rs, imm);
    break;
  case ADDIU:
    fprintf(stderr, "addiu r%d, r%d, %x\n", rt, rs, imm);
    break;
  case ANDI:
    fprintf(stderr, "andi r%d, r%d, %x\n", rt, rs, imm);
    break;
  case ORI:
    fprintf(stderr, "ori r%d, r%d, %x\n", rt, rs, imm);
    break;
  case XORI:
    fprintf(stderr, "xori r%d, r%d, %x\n", rt, rs, imm);
    break;
  case SLTI:
    fprintf(stderr, "slti r%d, r%d, %x\n", rt, rs, imm);
    break;
  case SLTIU:
    fprintf(stderr, "sltiu r%d, r%d, %x\n", rt, rs, imm);
    break;
  case BEQ:
    fprintf(stderr, "beq r%d, r%d, %x\n", rs, rt, imm);
    break;
  case BNE:
    fprintf(stderr, "bne r%d, r%d, %x\n", rs, rt, imm);
    break;
  case BGTZ:
    fprintf(stderr, "bgtz r%d, %x\n", rs, imm);
    break;
  case BLEZ:
    fprintf(stderr, "blez r%d, %x\n", rs, imm);
    break;
  case LB:
    fprintf(stderr, "lb r%d, %x\n", rt, imm);
    break;
  case LBU:
    fprintf(stderr, "lbu r%d, %x\n", rt, imm);
    break;
  case LH:
    fprintf(stderr, "lh r%d, %x\n", rt, imm);
    break;
  case LHU:
    fprintf(stderr, "lhu r%d, %x\n", rt, imm);
    break;
  case LW:
    fprintf(stderr, "lw r%d, %x\n", rt, imm);
    break;
  case LWL:
    fprintf(stderr, "lwl r%d, %x\n", rt, imm);
    break;
  case LWR:
    fprintf(stderr, "lwr r%d, %x\n", rt, imm);
    break;
  case SB:
    fprintf(stderr, "sb r%d, %x\n", rt, imm);
    break;
  case SH:
    fprintf(stderr, "sh r%d, %x\n", rt, imm);
    break;
  case SW:
    fprintf(stderr, "sw r%d, %x\n", rt, imm);
    break;
  case SWL:
    fprintf(stderr, "swl r%d, %x\n", rt, imm);
    break;
  case SWR:
    fprintf(stderr, "swr r%d, %x\n", rt, imm);
    break;
  case LUI:
    fprintf(stderr, "lui r%d, %x\n", rt, imm);
    break;
  default:
    perror("Bad immediate instruction\n");
  }
}
