#include "instructions.h"

#include <functions.h>
#include <stdio.h>

#include "logger.h"
#include "syscalls.h"
#include "utils.h"

extern struct global *glob;

void pc_step(int step)
{
  glob->pc += step;
}

void exec_register(uint32_t *instruction)
{
  uint32_t function = (*instruction) << 26;
  function = function >> 26;
  uint32_t rs = ((*instruction) >> 21) & 0x1F;
  uint32_t rt = ((*instruction) >> 16) & 0x1F;
  uint32_t rd = ((*instruction) >> 11) & 0x1F;
  uint32_t sa = ((*instruction) >> 6) & 0x1F;
  if (glob->debug)
    log_reg_instr(instruction);
  if (rd == 0x0 && function != JR && function != JALR)
  {
    perror("Cannot overwrite R0\n");
    return;
  }
  switch (function)
  {
  case ADD:
    add(rs, rt, rd);
    pc_step(4);
    break;
  case ADDU:
    glob->reg[rd] = glob->reg[rs] + glob->reg[rt];
    pc_step(4);
    break;
  case AND:
    glob->reg[rd] = glob->reg[rs] & glob->reg[rt];
    pc_step(4);
    break;
  case DIV:
    glob->hi = glob->reg[rs] % glob->reg[rt];
    glob->lo = glob->reg[rs] / glob->reg[rt];
    glob->reg[rd] = glob->lo;
    pc_step(4);
    break;
  case DIVU:
    glob->hi = glob->reg[rs] % glob->reg[rt];
    glob->lo = glob->reg[rs] / glob->reg[rt];
    glob->reg[rd] = glob->lo;
    pc_step(4);
    break;
  case MULT:
    glob->reg[rd] = glob->reg[rs] * glob->reg[rt];
    pc_step(4);
    break;
  case MULTU:
    glob->reg[rd] = glob->reg[rs] * glob->reg[rt];
    pc_step(4);
    break;
  case NOR:
    glob->reg[rd] = ~(glob->reg[rs] | glob->reg[rt]);
    pc_step(4);
    break;
  case OR:
    glob->reg[rd] = glob->reg[rs] | glob->reg[rt];
    pc_step(4);
    break;
  case SLL:
    glob->reg[rd] = glob->reg[rt] << sa;
    pc_step(4);
    break;
  case SLLV:
    glob->reg[rd] = glob->reg[rt] << (glob->reg[rs] & 0x1F);
    pc_step(4);
    break;
  case SRA:
    break;
  case SRAV:
    break;
  case SRL:
    glob->reg[rd] = glob->reg[rt] >> sa;
    pc_step(4);
    break;
  case SRLV:
    glob->reg[rd] = glob->reg[rt] >> (glob->reg[rs] & 0x1F);
    pc_step(4);
    break;
  case SUB:
    sub(rs, rt, rd);
    pc_step(4);
    break;
  case SUBU:
    glob->reg[rd] = glob->reg[rs] - glob->reg[rt];
    pc_step(4);
    break;
  case XOR:
    glob->reg[rd] = glob->reg[rs] ^ glob->reg[rt];
    pc_step(4);
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

void exec_immediate(uint32_t *instruction)
{
  uint8_t opcode = (*instruction) >> 26;
  uint32_t rs = ((*instruction) >> 21) & 0x1F;
  uint32_t rt = ((*instruction) >> 16) & 0x1F;
  uint32_t imm = (*instruction) & 0xFFFF;
  if (glob->debug)
    log_imm_instr(instruction);
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
    pc_step(4);
    break;
  case XORI:
    break;
  case SLTI:
    break;
  case SLTIU:
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
  case LUI:
    glob->reg[rt] = imm << 16;
    pc_step(4);
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
void exec_jump(uint32_t *instruction)
{
  uint8_t opcode = (*instruction) >> 26;
  uint32_t rs = (*instruction) & 0x3FFFFFF;
  switch (opcode)
  {
  case J:
    glob->pc = (glob->pc & 0xF0000000) | (rs << 2);
    break;
  case JAL:
    glob->reg[RA] = glob->pc + 4;
    glob->pc = (glob->pc & 0xF0000000) | (rs << 2);
    break;
  case TRAP:
    break;
  default:
    perror("Bad jump instruction\n");
    break;
  }
}

int branch_or_jump(uint32_t *instruction)
{
  uint8_t opcode = (*instruction) >> 26;
  uint32_t function = (*instruction) << 26;
  function = function >> 26;
  if (opcode == J || opcode == JAL)
    return 1;
  if (opcode == BEQ || opcode == BNE || opcode == BGTZ || opcode == BLEZ)
    return 2;
  if (opcode == 0)
  {
    if (function == JR || function == JALR)
      return 3;
  }
  return 0;
}

int exec_inst(uint32_t *instru)
{
  if (glob->debug)
  {}
  if (*instru == 0 || *instru == 0xa)
  {
    pc_step(4);
    return 0;
  }
  int b_or_j = branch_or_jump(instru);
  if (b_or_j)
  {
    uint32_t *next_instru = ((uint32_t *)glob->memory) + (glob->pc + 4) / 4;
    exec_inst(next_instru);
    if (b_or_j == 1)
      exec_jump(instru);
    if (b_or_j == 2)
      exec_immediate(instru);
    if (b_or_j == 3)
      exec_register(instru);
    return 0;
  }
  if (*instru == 0xc)
  {
    if (call_syscall())
      return 1;
    pc_step(4);
    return 0;
  }

  uint8_t opcode = (*instru) >> 26;
  if (opcode == 0)
    exec_register(instru);
  else if (opcode == J || opcode == JAL || opcode == TRAP)
    exec_jump(instru);
  else
    exec_immediate(instru);

  return 0;
}

void execute(void)
{
  while (1)
  {
    uint32_t *instru = ((uint32_t *)glob->memory) + (glob->pc / 4);
    if (exec_inst(instru))
      return;
  }
}
