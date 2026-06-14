#include "cpu.h"

#include <functions.h>

#include "logger.h"
#include "syscalls.h"
#include "utils.h"
#include <stdio.h>

extern struct global *glob;

/* Branch-delay pipeline: a taken branch/jump records its target here and
 * still lets the delay-slot instruction execute. The transfer is applied
 * one instruction later, in execute(). */
static int branch_pending = 0;
static uint32_t branch_target = 0;

void pc_step(int step)
{
  glob->pc += step;
}

static void schedule_branch(uint32_t target)
{
  branch_pending = 1;
  branch_target = target;
}

/* Branch offset is signed and relative to the delay slot (pc + 4). */
static uint32_t branch_dest(uint16_t imm)
{
  return glob->pc + 4 + ((int16_t)imm << 2);
}

int exec_register(uint32_t *instruction)
{
  uint32_t function = (*instruction) << 26;
  function = function >> 26;
  uint8_t rs = ((*instruction) >> 21) & 0x1F;
  uint8_t rt = ((*instruction) >> 16) & 0x1F;
  uint8_t rd = ((*instruction) >> 11) & 0x1F;
  uint8_t sa = ((*instruction) >> 6) & 0x1F;
  if (glob->log)
  {
    log_instr(instruction, __FILENAME__, __LINE__);
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
    glob->reg[rd] = (int32_t)glob->reg[rt] >> sa;
    pc_step(4);
    break;
  case SRAV:
    glob->reg[rd] = (int32_t)glob->reg[rt] >> (glob->reg[rs] & 0x1F);
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
    glob->reg[rd] = glob->pc + 8;
    schedule_branch(glob->reg[rs]);
    pc_step(4);
    break;
  case JR:
    schedule_branch(glob->reg[rs]);
    pc_step(4);
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
  glob->reg[R0] = 0;
  return 0;
}

int exec_immediate(uint32_t *instruction)
{
  uint8_t opcode = (*instruction) >> 26;
  uint8_t rs = ((*instruction) >> 21) & 0x1F;
  uint8_t rt = ((*instruction) >> 16) & 0x1F;
  uint16_t imm = (*instruction) & 0xFFFF;
  if (glob->log)
  {
    log_instr(instruction, __FILENAME__, __LINE__);
  }
  switch (opcode)
  {
  case ADDI:
    addi(rs, (int16_t)imm, rt);
    pc_step(4);
    break;
  case ADDIU:
    addiu(rs, (int16_t)imm, rt);
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
    slti(rs, (int16_t)imm, rt);
    pc_step(4);
    break;
  case SLTIU:
    sltiu(rs, (uint32_t)(int16_t)imm, rt);
    pc_step(4);
    break;
  case BEQ:
    if (glob->reg[rs] == glob->reg[rt])
      schedule_branch(branch_dest(imm));
    pc_step(4);
    break;
  case BNE:
    if (glob->reg[rs] != glob->reg[rt])
      schedule_branch(branch_dest(imm));
    pc_step(4);
    break;
  case BGTZ:
    if ((int32_t)glob->reg[rs] > 0)
      schedule_branch(branch_dest(imm));
    pc_step(4);
    break;
  case BLEZ:
    if ((int32_t)glob->reg[rs] <= 0)
      schedule_branch(branch_dest(imm));
    pc_step(4);
    break;
  /* REGIMM (opcode 0x01): bltz / bgez / bltzal / bgezal selected by rt. */
  case BGEZ:
    {
      int negative = (int32_t)glob->reg[rs] < 0;
      int take = (rt == BGEZ || rt == BGEZAL) ? !negative : negative;
      if (rt == BGEZAL || rt == BLTZAL)
        glob->reg[RA] = glob->pc + 8;
      if (take)
        schedule_branch(branch_dest(imm));
      pc_step(4);
      break;
    }
  case LB:
    {
      uint32_t addr = glob->reg[rs] + (int16_t)imm;
      glob->reg[rt] = (int32_t)(int8_t)load_byte(addr);
      pc_step(4);
      break;
    }
  case LBU:
    {
      uint32_t addr = glob->reg[rs] + (int16_t)imm;
      glob->reg[rt] = load_byte(addr);
      pc_step(4);
      break;
    }
  case LH:
    {
      uint32_t addr = glob->reg[rs] + (int16_t)imm;
      glob->reg[rt] = (int32_t)(int16_t)load_half(addr);
      pc_step(4);
      break;
    }
  case LHU:
    {
      uint32_t addr = glob->reg[rs] + (int16_t)imm;
      glob->reg[rt] = load_half(addr);
      pc_step(4);
      break;
    }
  case LW:
    {
      uint32_t addr = glob->reg[rs] + (int16_t)imm;
      glob->reg[rt] = load_word(addr);
      pc_step(4);
      break;
    }
  case LWL:
    {
      uint32_t addr = glob->reg[rs] + (int16_t)imm;
      uint32_t w = load_word(addr & ~3u);
      uint32_t shift = 24 - 8 * (addr & 3u);
      glob->reg[rt] = (w << shift) | (glob->reg[rt] & ((1u << shift) - 1));
      pc_step(4);
      break;
    }
  case LWR:
    {
      uint32_t addr = glob->reg[rs] + (int16_t)imm;
      uint32_t w = load_word(addr & ~3u);
      uint32_t shift = 8 * (addr & 3u);
      glob->reg[rt] = (w >> shift) | (glob->reg[rt] & ~(0xFFFFFFFFu >> shift));
      pc_step(4);
      break;
    }
  case SB:
    {
      uint32_t addr = glob->reg[rs] + (int16_t)imm;
      store_byte(addr, glob->reg[rt]);
      pc_step(4);
      break;
    }
  case SH:
    {
      uint32_t addr = glob->reg[rs] + (int16_t)imm;
      store_half(addr, glob->reg[rt]);
      pc_step(4);
      break;
    }
  case SW:
    {
      uint32_t addr = glob->reg[rs] + (int16_t)imm;
      store_word(addr, glob->reg[rt]);
      pc_step(4);
      break;
    }
  case SWL:
    {
      uint32_t addr = glob->reg[rs] + (int16_t)imm;
      uint32_t aligned = addr & ~3u;
      uint32_t w = load_word(aligned);
      uint32_t shift = 24 - 8 * (addr & 3u);
      store_word(aligned,
                 (glob->reg[rt] >> shift) | (w & ~(0xFFFFFFFFu >> shift)));
      pc_step(4);
      break;
    }
  case SWR:
    {
      uint32_t addr = glob->reg[rs] + (int16_t)imm;
      uint32_t aligned = addr & ~3u;
      uint32_t w = load_word(aligned);
      uint32_t shift = 8 * (addr & 3u);
      store_word(aligned,
                 (glob->reg[rt] << shift) | (w & ((1u << shift) - 1)));
      pc_step(4);
      break;
    }
  case LUI:
    glob->reg[rt] = imm << 16;
    pc_step(4);
    break;
  default:
    return 1;
  }
  glob->reg[R0] = 0;
  return 0;
}
int exec_jump(uint32_t *instruction)
{
  uint8_t opcode = (*instruction) >> 26;
  uint32_t rs = (*instruction) & 0x3FFFFFF;
  if (glob->log)
  {
    log_instr(instruction, __FILENAME__, __LINE__);
  }
  uint32_t target = (glob->pc & 0xF0000000) | (rs << 2);
  switch (opcode)
  {
  case J:
    schedule_branch(target);
    pc_step(4);
    break;
  case JAL:
    glob->reg[RA] = glob->pc + 8;
    schedule_branch(target);
    pc_step(4);
    break;
  default:
    return 1;
  }
  return 0;
}

int exec_inst(uint32_t *instru)
{
  uint8_t opcode = (*instru) >> 26;
  uint8_t function = (*instru) & 0x3F;

  if (*instru == 0) /* nop */
  {
    pc_step(4);
    return 0;
  }
  if (opcode == 0 && function == SYSCALL)
  {
    if (glob->log)
      log_instr(instru, __FILENAME__, __LINE__);
    if (call_syscall())
      return 2; /* exit / exit2: stop cleanly */
    pc_step(4);
    return 0;
  }
  if (opcode == 0 && function == BREAK)
  {
    if (glob->log)
      log_instr(instru, __FILENAME__, __LINE__);
    glob->reg[K0] = 1; /* signal the trap, then stop */
    return 2;
  }

  switch (opcode)
  {
  case 0:
    return exec_register(instru);
  case J:
  case JAL:
    return exec_jump(instru);
  default:
    return exec_immediate(instru);
  }
}

int execute(void)
{
  while (1)
  {
    uint32_t *instru = ((uint32_t *)glob->memory) + (glob->pc / 4);

    /* A branch scheduled by the previous instruction takes effect now,
     * after its delay slot (the current instruction) has run. */
    int take = branch_pending;
    uint32_t target = branch_target;
    branch_pending = 0;

    int ret = exec_inst(instru);
    if (ret == 1)
      return 1; /* decode/execution error */
    if (ret == 2)
      return 0; /* clean stop (exit / break) */

    if (take)
      glob->pc = target;
  }
}
