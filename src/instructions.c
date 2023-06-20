#include "instructions.h"

#include <stdio.h>

#include "syscalls.h"
#include "utils.h"

extern struct global *glob;

void parse_register(uint32_t *instruction)
{
  uint32_t function = (*instruction) << 26;
  function = function >> 26;
  uint32_t rs = ((*instruction) >> 21) & 0x1F;
  uint32_t rt = ((*instruction) >> 16) & 0x1F;
  uint32_t rd = ((*instruction) >> 11) & 0x1F;
  uint32_t sa = ((*instruction) >> 6) & 0x1F;
  if (rd == 0x0)
  {
    perror("Cannot overwrite R0\n");
    return;
  }
  switch (function)
  {
  case ADD:
    glob->reg[rd] = glob->reg[rs] + glob->reg[rt];
    break;
  case ADDU:
    glob->reg[rd] = glob->reg[rs] + glob->reg[rt];
    break;
  case AND:
    glob->reg[rd] = glob->reg[rs] & glob->reg[rt];
    break;
  case DIV:
  case DIVU:
  case MULT:
    glob->reg[rd] = glob->reg[rs] * glob->reg[rt];
    break;
  case MULTU:
    glob->reg[rd] = glob->reg[rs] * glob->reg[rt];
    break;
  case NOR:
    glob->reg[rd] = ~(glob->reg[rs] | glob->reg[rt]);
    break;
  case OR:
    glob->reg[rd] = glob->reg[rs] | glob->reg[rt];
    break;
  case SLL:
  case SLLV:
  case SRA:
  case SRAV:
  case SRL:
  case SRLV:
  case SUB:
  case SUBU:
  case XOR:
    glob->reg[rd] = glob->reg[rs] ^ glob->reg[rt];
    break;
  case SLT:
  case SLTU:
  case JALR:
  case JR:
    glob->pc = glob->reg[rs];
    break;
  case MFHI:
  case MFLO:
  case MTHI:
  case MTLO:
  default:
    perror("Bad register instruction\n");
    break;
  }
}

void parse_immediate(uint32_t *instruction)
{
  uint8_t opcode = (*instruction) >> 26;
  uint32_t rs = ((*instruction) >> 21) & 0x1F;
  uint32_t rt = ((*instruction) >> 16) & 0x1F;
  uint32_t imm = (*instruction) & 0xFFFF;
  switch (opcode)
  {
  case ADDI:
    break;
  case ADDIU:
    break;
  case ANDI:
    break;
  case ORI:
    glob->reg[rt] = glob->reg[rs] | imm;
    break;
  case XORI:
    break;
  case SLTI:
    break;
  case SLTIU:
    break;
  case LHI:
    break;
  case LLO:
    break;
  case BEQ:
    break;
  case BNE:
    break;
  case BGTZ:
    break;
  case BLEZ:
    break;
  case LB:
    break;
  case LBU:
    break;
  case LH:
    break;
  case LHU:
    break;
  case LW:
    break;
  case SB:
    break;
  case SH:
    break;
  case SW:
    break;
  default:
    perror("Bad immediate instruction\n");
  }
}
void parse_jump(uint32_t *instruction)
{
  uint8_t opcode = (*instruction) >> 26;
  uint32_t rs = (*instruction) & 0x3FFFFFF;
  switch (opcode)
  {
  case J:
    glob->pc = (glob->pc & 0xF0000000) | (rs << 2);
    break;
  case JAL:
    glob->pc = (glob->pc & 0xF0000000) | (rs << 2);
    glob->reg[RA] = glob->pc + 4;
    break;
  case TRAP:
    break;
  default:
    perror("Bad jump instruction\n");
    break;
  }
}

void parse_inst()
{
  for (size_t i = 0; i < (MEM_SIZE / sizeof(uint32_t)); i++)
  {
    uint32_t *instru = ((uint32_t *)glob->memory) + i;
    printf("%lx : %08x\n", i * sizeof(uint32_t), *instru);
    if (*instru == 0 || *instru == 0xa)
      continue;
    if (*instru == 0xc)
    {
      if (call_syscall())
        return;
      continue;
    }
    uint8_t opcode = (*instru) >> 26;
    if (opcode == 0)
      parse_register(instru);
    else if (opcode == J || opcode == JAL || opcode == TRAP)
      parse_jump(instru);
    else
      parse_immediate(instru);
  }
}
