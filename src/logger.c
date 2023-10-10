#include "logger.h"

#include <stdint.h>
#include <stdio.h>

#include "utils.h"

extern struct global *glob;

void log_reg_instr(uint32_t *instru, char *filename, int line)
{
  uint32_t function = (*instru) << 26;
  function = function >> 26;
  uint32_t rs = ((*instru) >> 21) & 0x1F;
  uint32_t rt = ((*instru) >> 16) & 0x1F;
  uint32_t rd = ((*instru) >> 11) & 0x1F;
  uint32_t sa = ((*instru) >> 6) & 0x1F;
  fprintf(stderr, "[my_mips] Executing pc = 0x%08x: 0x%08x: ", glob->pc, *instru);
  switch (function)
  {
  case ADD:
    fprintf(stderr, "add r%d, r%d, r%d", rd, rs, rt);
    break;
  case ADDU:
    fprintf(stderr, "addu r%d, r%d, r%d", rd, rs, rt);
    break;
  case DIV:
    fprintf(stderr, "div r%d, r%d", rs, rt);
    break;
  case DIVU:
    fprintf(stderr, "divu r%d, r%d", rs, rt);
    break;
  case MULT:
    fprintf(stderr, "mult r%d, r%d, r%d", rd, rs, rt);
    break;
  case MULTU:
    fprintf(stderr, "multu r%d, r%d, r%d", rd, rs, rt);
    break;
  case SUB:
    fprintf(stderr, "sub r%d, r%d, r%d", rd, rs, rt);
    break;
  case SUBU:
    fprintf(stderr, "subu r%d, r%d, r%d", rd, rs, rt);
    break;
  case AND:
    fprintf(stderr, "and r%d, r%d, r%d", rd, rs, rt);
    break;
  case NOR:
    fprintf(stderr, "nor r%d, r%d, r%d", rd, rs, rt);
    break;
  case OR:
    fprintf(stderr, "or r%d, r%d, r%d", rd, rs, rt);
    break;
  case SLT:
    fprintf(stderr, "slt r%d, r%d, r%d", rd, rs, rt);
    break;
  case SLTU:
    fprintf(stderr, "sltu r%d, r%d, r%d", rd, rs, rt);
    break;
  case XOR:
    fprintf(stderr, "xor r%d, r%d, r%d", rd, rs, rt);
    break;
  case SLL:
    fprintf(stderr, "sll r%d, r%d, r%d", rd, rt, sa);
    break;
  case SLLV:
    fprintf(stderr, "sllv r%d, r%d, r%d", rd, rt, rs);
    break;
  case SRA:
    fprintf(stderr, "sra r%d, r%d, r%d", rd, rt, sa);
    break;
  case SRAV:
    fprintf(stderr, "srav r%d, r%d, r%d", rd, rt, rs);
    break;
  case SRL:
    fprintf(stderr, "srl r%d, r%d, r%d", rd, rt, sa);
    break;
  case SRLV:
    fprintf(stderr, "srlv r%d, r%d, r%d", rd, rt, rs);
    break;
  case JALR:
    fprintf(stderr, "jalr r%d, r%d", rd, rs);
    break;
  case JR:
    fprintf(stderr, "jr r%d", rs);
    break;
  case MFHI:
    fprintf(stderr, "mfhi r%d", rd);
    break;
  case MFLO:
    fprintf(stderr, "mflo r%d", rd);
    break;
  case MTHI:
    fprintf(stderr, "mthi r%d", rs);
    break;
  case MTLO:
    fprintf(stderr, "mtlo r%d", rs);
    break;
  default:
    fprintf(stderr, "Bad register instruction");
    break;
  }
  fprintf(stderr, " (%s:%d)\n", filename, line);
}

void log_imm_instr(uint32_t *instru, char *filename, int line)
{
  uint8_t opcode = (*instru) >> 26;
  uint32_t rs = ((*instru) >> 21) & 0x1F;
  uint32_t rt = ((*instru) >> 16) & 0x1F;
  uint32_t imm = (*instru) & 0xFFFF;
  fprintf(stderr, "[my_mips] Executing pc = 0x%08x: 0x%08x: ", glob->pc, *instru);
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
    fprintf(stderr, "addi r%d, r%d, %x", rt, rs, imm);
    break;
  case ADDIU:
    fprintf(stderr, "addiu r%d, r%d, %x", rt, rs, imm);
    break;
  case ANDI:
    fprintf(stderr, "andi r%d, r%d, %x", rt, rs, imm);
    break;
  case ORI:
    fprintf(stderr, "ori r%d, r%d, %x", rt, rs, imm);
    break;
  case XORI:
    fprintf(stderr, "xori r%d, r%d, %x", rt, rs, imm);
    break;
  case SLTI:
    fprintf(stderr, "slti r%d, r%d, %x", rt, rs, imm);
    break;
  case SLTIU:
    fprintf(stderr, "sltiu r%d, r%d, %x", rt, rs, imm);
    break;
  case BEQ:
    fprintf(stderr, "beq r%d, r%d, %x", rs, rt, imm);
    break;
  case BNE:
    fprintf(stderr, "bne r%d, r%d, %x", rs, rt, imm);
    break;
  case BGTZ:
    fprintf(stderr, "bgtz r%d, %x", rs, imm);
    break;
  case BLEZ:
    fprintf(stderr, "blez r%d, %x", rs, imm);
    break;
  case LB:
    fprintf(stderr, "lb r%d, %x", rt, imm);
    break;
  case LBU:
    fprintf(stderr, "lbu r%d, %x", rt, imm);
    break;
  case LH:
    fprintf(stderr, "lh r%d, %x", rt, imm);
    break;
  case LHU:
    fprintf(stderr, "lhu r%d, %x", rt, imm);
    break;
  case LW:
    fprintf(stderr, "lw r%d, %x", rt, imm);
    break;
  case LWL:
    fprintf(stderr, "lwl r%d, %x", rt, imm);
    break;
  case LWR:
    fprintf(stderr, "lwr r%d, %x", rt, imm);
    break;
  case SB:
    fprintf(stderr, "sb r%d, %x", rt, imm);
    break;
  case SH:
    fprintf(stderr, "sh r%d, %x", rt, imm);
    break;
  case SW:
    fprintf(stderr, "sw r%d, %x", rt, imm);
    break;
  case SWL:
    fprintf(stderr, "swl r%d, %x", rt, imm);
    break;
  case SWR:
    fprintf(stderr, "swr r%d, %x", rt, imm);
    break;
  case LUI:
    fprintf(stderr, "lui r%d, %x", rt, imm);
    break;
  default:
    fprintf(stderr, "Bad immediate instruction");
  }
  fprintf(stderr, " (%s:%d)\n", filename, line);
}

void log_jump_instr(uint32_t *instru, char *filename, int line)
{
  uint8_t opcode = (*instru) >> 26;
  uint32_t rs = (*instru) & 0x3FFFFFF;

  fprintf(stderr, "[my_mips] Executing pc = 0x%08x: 0x%08x: ", glob->pc, *instru);
  switch (opcode)
  {
    case J:
      fprintf(stderr, "j 0x%x", rs << 2);
      break;
    case JAL:
      fprintf(stderr, "jal 0x%x", rs << 2);
      break;
    case TRAP:
      break;
    default:
      fprintf(stderr, "Bad jump instruction");
  }
  fprintf(stderr, " (%s:%d)\n", filename, line);
}

void log_syscall(uint32_t *instru, char *filename, int line)
{
  fprintf(stderr, "[my_mips] Executing pc = 0x%08x: 0x%08x: syscall (%s:%d)\n", glob->pc, *instru, filename, line);
}
