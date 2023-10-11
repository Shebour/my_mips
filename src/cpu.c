#include "cpu.h"

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

int exec_register(uint32_t *instruction)
{
  uint32_t function = (*instruction) << 26;
  function = function >> 26;
  uint8_t rs = ((*instruction) >> 21) & 0x1F;
  uint8_t rt = ((*instruction) >> 16) & 0x1F;
  uint8_t rd = ((*instruction) >> 11) & 0x1F;
  uint8_t sa = ((*instruction) >> 6) & 0x1F;
  if (rd == 0x0 && function != JR && function != JALR)
  {
    perror("Cannot overwrite R0\n");
    return 1;
  }
  switch (function)
  {
  case ADD:
    add(rs, rt, rd);
    pc_step(4);
    break;
  case ADDU:
    addu(rs, rt, rd);
    pc_step(4);
    break;
  case AND:
    glob->reg[rd] = glob->reg[rs] & glob->reg[rt];
    pc_step(4);
    break;
  case DIV:
    div(rs, rt);
    pc_step(4);
    break;
  case DIVU:
    divu(rs, rt);
    pc_step(4);
    break;
  case MULT:
    mult(rs, rt);
    pc_step(4);
    break;
  case MULTU:
    multu(rs, rt);
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
    glob->reg[rt] >>= sa;
    glob->reg[rd] = glob->reg[rt];
    pc_step(4);
    break;
  case SRAV:
    rs <<= 3;
    rs >>= 3;
    glob->reg[rt] >>= rs;
    glob->reg[rd] = glob->reg[rt];
    pc_step(4);
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
    subu(rs, rt, rd);
    pc_step(4);
    break;
  case XOR:
    glob->reg[rd] = glob->reg[rs] ^ glob->reg[rt];
    pc_step(4);
    break;
  case SLT:
    slt(rs, rt, rd);
    pc_step(4);
    break;
  case SLTU:
    sltu(rs, rt, rd);
    pc_step(4);
    break;
  case JALR:
  case JR:
    glob->pc = glob->reg[rs];
    break;
  case MFHI:
    glob->reg[rd] = glob->hi;
    pc_step(4);
    break;
  case MFLO:
    glob->reg[rd] = glob->lo;
    pc_step(4);
    break;
  case MTHI:
    glob->hi = glob->reg[rs];
    pc_step(4);
    break;
  case MTLO:
    glob->lo = glob->reg[rs];
    pc_step(4);
    break;
  default:
    return 1;
  }
  return 0;
}

int exec_immediate(uint32_t *instruction)
{
  uint8_t opcode = (*instruction) >> 26;
  uint32_t rs = ((*instruction) >> 21) & 0x1F;
  uint32_t rt = ((*instruction) >> 16) & 0x1F;
  uint32_t imm = (*instruction) & 0xFFFF;
  switch (opcode)
  {
  case ADDI:
    addi(rs, imm, rt);
    pc_step(4);
    break;
  case ADDIU:
    addiu(rs, imm, rt);
    pc_step(4);
    break;
  case ANDI:
    glob->reg[rt] = glob->reg[rs] & imm;
    pc_step(4);
    break;
  case ORI:
    glob->reg[rt] = glob->reg[rs] | imm;
    pc_step(4);
    break;
  case XORI:
    glob->reg[rt] = glob->reg[rs] ^ imm;
    pc_step(4);
    break;
  case SLTI:
    slti(rs, imm, rt);
    pc_step(4);
    break;
  case SLTIU:
    sltiu(rs, imm, rt);
    pc_step(4);
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
    break;
  case SB:
    break;
  case SH:
    break;
  case SW:
    break;
  default:
    return 1;
  }
  return 0;
}
int exec_jump(uint32_t *instruction)
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
    return 1;
  }
  return 0;
}

int instruction_type(uint32_t *instruction)
{
  uint8_t opcode = (*instruction) >> 26;
  uint32_t function = (*instruction) << 26;
  function = function >> 26;
  if (opcode == J || opcode == JAL)
    return JUMP;
  if (opcode == BEQ || opcode == BNE || opcode == BGTZ || opcode == BLEZ)
    return IMM;
  if (opcode == 0)
  {
    if (function == JR || function == JALR)
      return REG;
  }
  return 0;
}

int exec_inst(uint32_t *instru)
{
  if (*instru == 0 || *instru == 0xa)
  {
    pc_step(4);
    return 0;
  }
  if (glob->debug)
  {
    log_instr(instru, __FILENAME__, __LINE__);
  }
  int inst_type = instruction_type(instru);
  if (inst_type)
  {
    uint32_t *next_instru = ((uint32_t *)glob->memory) + (glob->pc + 4) / 4;
    exec_inst(next_instru);
    if (inst_type == JUMP)
      exec_jump(instru);
    if (inst_type == IMM)
      exec_immediate(instru);
    if (inst_type == REG)
      exec_register(instru);
    return 0;
  }
  if (*instru == 0xc)
  {
    if (call_syscall())
      return 2;
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

int execute(void)
{
  while (1)
  {
    uint32_t *instru = ((uint32_t *)glob->memory) + (glob->pc / 4);
    int ret = exec_inst(instru);
    if (ret)
      return ret;
  }
}
