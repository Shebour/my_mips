#include "cpu.h"

#include "fpu.h"
#include "functions.h"
#include "logger.h"
#include "syscalls.h"
#include "utils.h"

extern struct global *glob;

/* Branch-delay pipeline: a taken branch/jump records its target here and
 * still lets the delay-slot instruction execute. execute() applies the
 * transfer one instruction later, after the delay slot has run. */
static int branch_pending = 0;
static uint32_t branch_target = 0;

void pc_step(int step)
{
  glob->pc += step;
}

void schedule_branch(uint32_t target)
{
  branch_pending = 1;
  branch_target = target;
}

/* True while a branch is waiting for its delay slot to run. The JIT consults
 * this so it routes delay slots through the interpreter (which applies the
 * branch), and only compiles blocks when no transfer is pending. */
int branch_is_pending(void)
{
  return branch_pending;
}

/* Target of a taken branch: signed offset relative to the delay slot. */
uint32_t branch_dest(uint32_t i)
{
  return glob->pc + 4 + ((int16_t)imm16(i) << 2);
}

/* Effective address of a load/store: base register + signed offset. */
static uint32_t mem_addr(uint32_t i)
{
  return glob->reg[rs(i)] + (int16_t)imm16(i);
}

/* R-type (SPECIAL, opcode 0): arithmetic, logic, shifts, HI/LO, jr/jalr. */
int exec_register(uint32_t *instruction)
{
  uint32_t i = *instruction;
  switch (funct(i))
  {
  case ADD: add(i); break;
  case SUB: sub(i); break;
  case ADDU: glob->reg[rd(i)] = glob->reg[rs(i)] + glob->reg[rt(i)]; break;
  case SUBU: glob->reg[rd(i)] = glob->reg[rs(i)] - glob->reg[rt(i)]; break;
  case MULT: mult(i); break;
  case MULTU: multu(i); break;
  case DIV: div(i); break;
  case DIVU: divu(i); break;
  case AND: glob->reg[rd(i)] = glob->reg[rs(i)] & glob->reg[rt(i)]; break;
  case OR: glob->reg[rd(i)] = glob->reg[rs(i)] | glob->reg[rt(i)]; break;
  case XOR: glob->reg[rd(i)] = glob->reg[rs(i)] ^ glob->reg[rt(i)]; break;
  case NOR: glob->reg[rd(i)] = ~(glob->reg[rs(i)] | glob->reg[rt(i)]); break;
  case SLT:
    glob->reg[rd(i)] = (int32_t)glob->reg[rs(i)] < (int32_t)glob->reg[rt(i)];
    break;
  case SLTU: glob->reg[rd(i)] = glob->reg[rs(i)] < glob->reg[rt(i)]; break;
  case SLL: glob->reg[rd(i)] = glob->reg[rt(i)] << shamt(i); break;
  case SRL: glob->reg[rd(i)] = glob->reg[rt(i)] >> shamt(i); break;
  case SRA: glob->reg[rd(i)] = (int32_t)glob->reg[rt(i)] >> shamt(i); break;
  case SLLV:
    glob->reg[rd(i)] = glob->reg[rt(i)] << (glob->reg[rs(i)] & 0x1F);
    break;
  case SRLV:
    glob->reg[rd(i)] = glob->reg[rt(i)] >> (glob->reg[rs(i)] & 0x1F);
    break;
  case SRAV:
    glob->reg[rd(i)] = (int32_t)glob->reg[rt(i)] >> (glob->reg[rs(i)] & 0x1F);
    break;
  case MFHI: glob->reg[rd(i)] = glob->hi; break;
  case MFLO: glob->reg[rd(i)] = glob->lo; break;
  case MTHI: glob->hi = glob->reg[rs(i)]; break;
  case MTLO: glob->lo = glob->reg[rs(i)]; break;
  case MOVZ:
    if (glob->reg[rt(i)] == 0)
      glob->reg[rd(i)] = glob->reg[rs(i)];
    break;
  case MOVN:
    if (glob->reg[rt(i)] != 0)
      glob->reg[rd(i)] = glob->reg[rs(i)];
    break;
  case JR: schedule_branch(glob->reg[rs(i)]); break;
  case JALR:
    glob->reg[rd(i)] = glob->pc + 8;
    schedule_branch(glob->reg[rs(i)]);
    break;
  default: return 1;
  }
  glob->reg[R0] = 0;
  return 0;
}

/* I-type: immediate arithmetic/logic, branches, loads and stores. */
int exec_immediate(uint32_t *instruction)
{
  uint32_t i = *instruction;
  switch (op(i))
  {
  case ADDI: addi(i); break;
  case ADDIU: glob->reg[rt(i)] = glob->reg[rs(i)] + (int16_t)imm16(i); break;
  case ANDI: glob->reg[rt(i)] = glob->reg[rs(i)] & imm16(i); break;
  case ORI: glob->reg[rt(i)] = glob->reg[rs(i)] | imm16(i); break;
  case XORI: glob->reg[rt(i)] = glob->reg[rs(i)] ^ imm16(i); break;
  case SLTI:
    glob->reg[rt(i)] = (int32_t)glob->reg[rs(i)] < (int16_t)imm16(i);
    break;
  case SLTIU:
    glob->reg[rt(i)] = glob->reg[rs(i)] < (uint32_t)(int16_t)imm16(i);
    break;
  case LUI: glob->reg[rt(i)] = imm16(i) << 16; break;
  case BEQ:
    if (glob->reg[rs(i)] == glob->reg[rt(i)])
      schedule_branch(branch_dest(i));
    break;
  case BNE:
    if (glob->reg[rs(i)] != glob->reg[rt(i)])
      schedule_branch(branch_dest(i));
    break;
  case BGTZ:
    if ((int32_t)glob->reg[rs(i)] > 0)
      schedule_branch(branch_dest(i));
    break;
  case BLEZ:
    if ((int32_t)glob->reg[rs(i)] <= 0)
      schedule_branch(branch_dest(i));
    break;
  case OP_REGIMM: /* bltz / bgez / bltzal / bgezal, selected by rt */
  {
    int negative = (int32_t)glob->reg[rs(i)] < 0;
    int take = (rt(i) == BGEZ || rt(i) == BGEZAL) ? !negative : negative;
    if (rt(i) == BGEZAL || rt(i) == BLTZAL)
      glob->reg[RA] = glob->pc + 8;
    if (take)
      schedule_branch(branch_dest(i));
    break;
  }
  case LB: glob->reg[rt(i)] = (int32_t)(int8_t)load_byte(mem_addr(i)); break;
  case LBU: glob->reg[rt(i)] = load_byte(mem_addr(i)); break;
  case LH: glob->reg[rt(i)] = (int32_t)(int16_t)load_half(mem_addr(i)); break;
  case LHU: glob->reg[rt(i)] = load_half(mem_addr(i)); break;
  case LW: glob->reg[rt(i)] = load_word(mem_addr(i)); break;
  case LWL:
  {
    uint32_t a = mem_addr(i);
    uint32_t w = load_word(a & ~3u);
    uint32_t s = 24 - 8 * (a & 3u);
    glob->reg[rt(i)] = (w << s) | (glob->reg[rt(i)] & ((1u << s) - 1));
    break;
  }
  case LWR:
  {
    uint32_t a = mem_addr(i);
    uint32_t w = load_word(a & ~3u);
    uint32_t s = 8 * (a & 3u);
    glob->reg[rt(i)] = (w >> s) | (glob->reg[rt(i)] & ~(0xFFFFFFFFu >> s));
    break;
  }
  case SB: store_byte(mem_addr(i), glob->reg[rt(i)]); break;
  case SH: store_half(mem_addr(i), glob->reg[rt(i)]); break;
  case SW: store_word(mem_addr(i), glob->reg[rt(i)]); break;
  case SWL:
  {
    uint32_t a = mem_addr(i);
    uint32_t al = a & ~3u;
    uint32_t s = 24 - 8 * (a & 3u);
    store_word(al,
               (glob->reg[rt(i)] >> s) | (load_word(al) & ~(0xFFFFFFFFu >> s)));
    break;
  }
  case SWR:
  {
    uint32_t a = mem_addr(i);
    uint32_t al = a & ~3u;
    uint32_t s = 8 * (a & 3u);
    store_word(al,
               (glob->reg[rt(i)] << s) | (load_word(al) & ((1u << s) - 1)));
    break;
  }
  default: return 1;
  }
  glob->reg[R0] = 0;
  return 0;
}

/* J-type: j and jal. */
int exec_jump(uint32_t *instruction)
{
  uint32_t i = *instruction;
  uint32_t target = (glob->pc & 0xF0000000) | (target26(i) << 2);
  switch (op(i))
  {
  case J: schedule_branch(target); break;
  case JAL:
    glob->reg[RA] = glob->pc + 8;
    schedule_branch(target);
    break;
  default: return 1;
  }
  return 0;
}

/* MIPS32 SPECIAL2 group (opcode 0x1c): mul, madd/u, msub/u, clz, clo. */
int exec_special2(uint32_t *instruction)
{
  uint32_t i = *instruction;
  switch (funct(i))
  {
  case MUL:
    glob->reg[rd(i)] = (int32_t)glob->reg[rs(i)] * (int32_t)glob->reg[rt(i)];
    break;
  case MADD:
  {
    int64_t acc = ((int64_t)(int32_t)glob->hi << 32) | (uint32_t)glob->lo;
    acc += (int64_t)(int32_t)glob->reg[rs(i)] * (int32_t)glob->reg[rt(i)];
    glob->hi = (uint32_t)(acc >> 32);
    glob->lo = (uint32_t)acc;
    break;
  }
  case MADDU:
  {
    uint64_t acc = ((uint64_t)glob->hi << 32) | glob->lo;
    acc += (uint64_t)glob->reg[rs(i)] * glob->reg[rt(i)];
    glob->hi = (uint32_t)(acc >> 32);
    glob->lo = (uint32_t)acc;
    break;
  }
  case MSUB:
  {
    int64_t acc = ((int64_t)(int32_t)glob->hi << 32) | (uint32_t)glob->lo;
    acc -= (int64_t)(int32_t)glob->reg[rs(i)] * (int32_t)glob->reg[rt(i)];
    glob->hi = (uint32_t)(acc >> 32);
    glob->lo = (uint32_t)acc;
    break;
  }
  case MSUBU:
  {
    uint64_t acc = ((uint64_t)glob->hi << 32) | glob->lo;
    acc -= (uint64_t)glob->reg[rs(i)] * glob->reg[rt(i)];
    glob->hi = (uint32_t)(acc >> 32);
    glob->lo = (uint32_t)acc;
    break;
  }
  case CLZ:
  {
    uint32_t v = glob->reg[rs(i)];
    uint32_t n = 0;
    while (n < 32 && !(v & 0x80000000u))
    {
      n++;
      v <<= 1;
    }
    glob->reg[rd(i)] = n;
    break;
  }
  case CLO:
  {
    uint32_t v = glob->reg[rs(i)];
    uint32_t n = 0;
    while (n < 32 && (v & 0x80000000u))
    {
      n++;
      v <<= 1;
    }
    glob->reg[rd(i)] = n;
    break;
  }
  default: return 1;
  }
  glob->reg[R0] = 0;
  return 0;
}

/* Decode one instruction and route it to the right execution group.
 * Returns 0 to continue, 1 on a decode error, 2 to stop cleanly. */
int exec_inst(uint32_t *instru)
{
  uint32_t i = *instru;
  if (glob->log)
    log_instr(instru, __FILENAME__, __LINE__);

  if (i == 0) /* nop */
    return 0;
  if (op(i) == OP_SPECIAL && funct(i) == SYSCALL)
    return call_syscall() ? 2 : 0; /* exit / exit2 stop cleanly */
  if (op(i) == OP_SPECIAL && funct(i) == BREAK)
  {
    glob->reg[K0] = 1; /* signal the trap, then stop */
    return 2;
  }

  switch (op(i))
  {
  case OP_SPECIAL: return exec_register(instru);
  case OP_SPECIAL2: return exec_special2(instru);
  case OP_COP1: return exec_cop1(instru);
  case LWC1:
  case LDC1:
  case SWC1:
  case SDC1: return exec_cop1_ldst(instru);
  case J:
  case JAL: return exec_jump(instru);
  default: return exec_immediate(instru);
  }
}

/* Execute exactly one instruction, applying the branch-delay pipeline.
 * Returns 0 to continue, 1 on a decode error, 2 to stop (exit / break).
 * Shared by execute() and the integrated debugger so both step identically. */
int step_instruction(void)
{
  uint32_t *instru = ((uint32_t *)glob->memory) + (glob->pc / 4);

  /* A branch scheduled by the previous instruction takes effect now, after
   * its delay slot (the instruction we are about to run) has executed. */
  int take = branch_pending;
  uint32_t target = branch_target;
  branch_pending = 0;

  int ret = exec_inst(instru);
  if (ret == 0)
  {
    pc_step(4);
    if (take)
      glob->pc = target;
  }
  return ret;
}

int execute(void)
{
  while (1)
  {
    int ret = step_instruction();
    if (ret == 1)
      return 1; /* decode/execution error */
    if (ret == 2)
      return 0; /* clean stop (exit / break) */
  }
}
