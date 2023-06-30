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
  printf("[my_mips] Executing pc = %08x: %08x: ", glob->pc, *instru);
  switch (function)
  {
  case ADD:
    printf("add r%d, r%d, r%d\n", rd, rs, rt);
    break;
  case ADDU:
    printf("addu r%d, r%d, r%d\n", rd, rs, rt);
    break;
  case DIV:
    printf("div r%d, r%d\n", rs, rt);
    break;
  case DIVU:
    printf("divu r%d, r%d\n", rs, rt);
    break;
  case MULT:
    printf("mult r%d, r%d, r%d\n", rd, rs, rt);
    break;
  case MULTU:
    printf("multu r%d, r%d, r%d\n", rd, rs, rt);
    break;
  case SUB:
    printf("sub r%d, r%d, r%d\n", rd, rs, rt);
    break;
  case SUBU:
    printf("subu r%d, r%d, r%d\n", rd, rs, rt);
    break;
  case AND:
    printf("and r%d, r%d, r%d\n", rd, rs, rt);
    break;
  case NOR:
    printf("nor r%d, r%d, r%d\n", rd, rs, rt);
    break;
  case OR:
    printf("or r%d, r%d, r%d\n", rd, rs, rt);
    break;
  case SLT:
    printf("slt r%d, r%d, r%d\n", rd, rs, rt);
    break;
  case SLTU:
    printf("sltu r%d, r%d, r%d\n", rd, rs, rt);
    break;
  case XOR:
    printf("xor r%d, r%d, r%d\n", rd, rs, rt);
    break;
  case SLL:
    printf("sll r%d, r%d, r%d\n", rd, rt, sa);
    break;
  case SLLV:
    printf("sllv r%d, r%d, r%d\n", rd, rt, rs);
    break;
  case SRA:
    printf("sra r%d, r%d, r%d\n", rd, rt, sa);
    break;
  case SRAV:
    printf("srav r%d, r%d, r%d\n", rd, rt, rs);
    break;
  case SRL:
    printf("srl r%d, r%d, r%d\n", rd, rt, sa);
    break;
  case SRLV:
    printf("srlv r%d, r%d, r%d\n", rd, rt, rs);
    break;
  case JALR:
    printf("jalr r%d, r%d\n", rd, rs);
    break;
  case JR:
    printf("jr r%d\n", rs);
    break;
  case MFHI:
    printf("mfhi r%d\n", rd);
    break;
  case MFLO:
    printf("mflo r%d\n", rd);
    break;
  case MTHI:
    printf("mthi r%d\n", rs);
    break;
  case MTLO:
    printf("mtlo r%d\n", rs);
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
  printf("[my_mips] Executing pc = %08x: %08x: ", glob->pc, *instru);
  if (opcode == 1)
  {
    switch (rt)
    {
    case BGEZ:
      printf("bgez r%d, %x\n", rs, imm);
      break;
    case BGEZAL:
      printf("bgezal r%d, %x\n", rs, imm);
      break;
    case BLTZ:
      printf("bltz r%d, %x\n", rs, imm);
      break;
    case BLTZAL:
      printf("bltz r%d, %x\n", rs, imm);
      break;
    }
  }
  switch (opcode)
  {
  case ADDI:
    printf("addi r%d, r%d, %x\n", rt, rs, imm);
    break;
  case ADDIU:
    printf("addiu r%d, r%d, %x\n", rt, rs, imm);
    break;
  case ANDI:
    printf("andi r%d, r%d, %x\n", rt, rs, imm);
    break;
  case ORI:
    printf("ori r%d, r%d, %x\n", rt, rs, imm);
    break;
  case XORI:
    printf("xori r%d, r%d, %x\n", rt, rs, imm);
    break;
  case SLTI:
    printf("slti r%d, r%d, %x\n", rt, rs, imm);
    break;
  case SLTIU:
    printf("sltiu r%d, r%d, %x\n", rt, rs, imm);
    break;
  case BEQ:
    printf("beq r%d, r%d, %x\n", rs, rt, imm);
    break;
  case BNE:
    printf("bne r%d, r%d, %x\n", rs, rt, imm);
    break;
  case BGTZ:
    printf("bgtz r%d, %x\n", rs, imm);
    break;
  case BLEZ:
    printf("blez r%d, %x\n", rs, imm);
    break;
  case LB:
    printf("lb r%d, %x\n", rt, imm);
    break;
  case LBU:
    printf("lbu r%d, %x\n", rt, imm);
    break;
  case LH:
    printf("lh r%d, %x\n", rt, imm);
    break;
  case LHU:
    printf("lhu r%d, %x\n", rt, imm);
    break;
  case LW:
    printf("lw r%d, %x\n", rt, imm);
    break;
  case LWL:
    printf("lwl r%d, %x\n", rt, imm);
    break;
  case LWR:
    printf("lwr r%d, %x\n", rt, imm);
    break;
  case SB:
    printf("sb r%d, %x\n", rt, imm);
    break;
  case SH:
    printf("sh r%d, %x\n", rt, imm);
    break;
  case SW:
    printf("sw r%d, %x\n", rt, imm);
    break;
  case SWL:
    printf("swl r%d, %x\n", rt, imm);
    break;
  case SWR:
    printf("swr r%d, %x\n", rt, imm);
    break;
  case LUI:
    printf("lui r%d, %x\n", rt, imm);
    break;
  default:
    perror("Bad immediate instruction\n");
  }
}
