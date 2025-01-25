#include "logger.h"

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#include "utils.h"

extern struct global *glob;

#define MAX_LOC_LEN 20

static void align_location(const char *file, int line, char *str)
{
  char line_str[7] = { 0 };
  int len_line = snprintf(line_str, 7, "%d", line);
  size_t len_filename = strlen(file);
  int len_total = len_line + len_filename + 1;
  if (len_total > MAX_LOC_LEN)
  {
    int off = len_total - MAX_LOC_LEN + 3;
    snprintf(str, MAX_LOC_LEN, "...%s:%s", file + off, line_str);
  }
  else
    snprintf(str, MAX_LOC_LEN, "%s:%s", file, line_str);
}

void logger(const char *file, int line, const char *fmt, ...)
{
  char str[MAX_LOC_LEN] = { 0 };
  align_location(file, line, str);
  va_list args_list;
  va_start(args_list, fmt);
  fprintf(stderr, "[my_mips] ");
  vfprintf(stderr, fmt, args_list);
  fprintf(stderr, " (%s)", str);
  fprintf(stderr, "\n");
  fflush(stderr);
  va_end(args_list);
}
void log_reg_instr(uint32_t *instru, char *filename, int line)
{
  uint32_t function = (*instru) << 26;
  function = function >> 26;
  uint32_t rs = ((*instru) >> 21) & 0x1F;
  uint32_t rt = ((*instru) >> 16) & 0x1F;
  uint32_t rd = ((*instru) >> 11) & 0x1F;
  uint32_t sa = ((*instru) >> 6) & 0x1F;
  fprintf(stderr, "[my_mips] Executing pc = 0x%08x: 0x%08x: ", glob->pc,
          *instru);
  switch (function)
  {
  case ADD:
    fprintf(stderr, "add r%d, r%d, r%d", rd, rs, rt);
    fprintf(stderr, " | add glob->reg[%d] = %x + %x", rd, glob->reg[rs], glob->reg[rt]);
    break;
  case ADDU:
    fprintf(stderr, "addu r%d, r%d, r%d", rd, rs, rt);
    fprintf(stderr, " | addu glob->reg[%d] = %x + %x", rd, glob->reg[rs], glob->reg[rt]);
    break;
  case DIV:
    fprintf(stderr, "div r%d, r%d", rs, rt);
    fprintf(stderr, " | div glob->reg[%d] = %x / %x", rd, glob->reg[rs], glob->reg[rt]);
    break;
  case DIVU:
    fprintf(stderr, "divu r%d, r%d", rs, rt);
    fprintf(stderr, " | divu %x / %x", glob->reg[rs], glob->reg[rt]);
    break;
  case MULT:
    fprintf(stderr, "mult r%d, r%d", rs, rt);
    fprintf(stderr, " | mult %x * %x", glob->reg[rs], glob->reg[rt]);
    break;
  case MULTU:
    fprintf(stderr, "multu r%d, r%d", rs, rt);
    fprintf(stderr, " | multu %x * %x", glob->reg[rs], glob->reg[rt]);
    break;
  case SUB:
    fprintf(stderr, "sub r%d, r%d, r%d", rd, rs, rt);
    fprintf(stderr, " | sub glob->reg[%d] = %x - %x", rd, glob->reg[rs], glob->reg[rt]);
    break;
  case SUBU:
    fprintf(stderr, "subu r%d, r%d, r%d", rd, rs, rt);
    fprintf(stderr, " | subu glob->reg[%d] = %x - %x", rd, glob->reg[rs], glob->reg[rt]);
    break;
  case AND:
    fprintf(stderr, "and r%d, r%d, r%d", rd, rs, rt);
    fprintf(stderr, " | and glob->reg[%d] = %x AND %x", rd, glob->reg[rs], glob->reg[rt]);
    break;
  case NOR:
    fprintf(stderr, "nor r%d, r%d, r%d", rd, rs, rt);
    fprintf(stderr, " | nor glob->reg[%d] = %x NOR %x", rd, glob->reg[rs], glob->reg[rt]);
    break;
  case OR:
    fprintf(stderr, "or r%d, r%d, r%d", rd, rs, rt);
    fprintf(stderr, " | or glob->reg[%d] = %x OR %x", rd, glob->reg[rs], glob->reg[rt]);
    break;
  case SLT:
    fprintf(stderr, "slt r%d, r%d, r%d", rd, rs, rt);
    fprintf(stderr, " | slt glob->reg[%d] = %x < %x", rd, glob->reg[rs], glob->reg[rt]);
    break;
  case SLTU:
    fprintf(stderr, "sltu r%d, r%d, r%d", rd, rs, rt);
    fprintf(stderr, " | sltu glob->reg[%d] = %x < %x", rd, glob->reg[rs], glob->reg[rt]);
    break;
  case XOR:
    fprintf(stderr, "xor r%d, r%d, r%d", rd, rs, rt);
    fprintf(stderr, " | xor glob->reg[%d] = %x XOR %x", rd, glob->reg[rs], glob->reg[rt]);
    break;
  case SLL:
    fprintf(stderr, "sll r%d, r%d, r%d", rd, rt, sa);
    fprintf(stderr, " | sll glob->reg[%d] = %x << %x", rd, glob->reg[rt], sa);
    break;
  case SLLV:
    fprintf(stderr, "sllv r%d, r%d, r%d", rd, rt, rs);
    fprintf(stderr, " | sllv glob->reg[%d] = %x << %x", rd, glob->reg[rt], glob->reg[rs]);
    break;
  case SRA:
    fprintf(stderr, "sra r%d, r%d, r%d", rd, rt, sa);
    fprintf(stderr, " | sra glob->reg[%d] = %x >> %x", rd, glob->reg[rt], sa);
    break;
  case SRAV:
    fprintf(stderr, "srav r%d, r%d, r%d", rd, rt, rs);
    fprintf(stderr, " | srav glob->reg[%d] = %x >> %x", rd, glob->reg[rt], glob->reg[rs]);
    break;
  case SRL:
    fprintf(stderr, "srl r%d, r%d, r%d", rd, rt, sa);
    fprintf(stderr, " | srl glob->reg[%d] = %x >> %x", rd, glob->reg[rt], sa);
    break;
  case SRLV:
    fprintf(stderr, "srlv r%d, r%d, r%d", rd, rt, rs);
    fprintf(stderr, " | srlv glob->reg[%d] = %x >> %x", rd, glob->reg[rt], glob->reg[rs]);
    break;
  case JALR:
    fprintf(stderr, "jalr r%d, r%d", rd, rs);
    fprintf(stderr, " | jalr glob->reg[%d] = %x, glob->pc = %x", rd, glob->reg[RA], glob->reg[rs]);
    break;
  case JR:
    fprintf(stderr, "jr r%d", rs);
    fprintf(stderr, " | jr glob->pc = 0x%x", glob->reg[rs]);
    break;
  case MFHI:
    fprintf(stderr, "mfhi r%d", rd);
    fprintf(stderr, " | mfhi glob->reg[%d] = %x", rd, glob->hi);
    break;
  case MFLO:
    fprintf(stderr, "mflo r%d", rd);
    fprintf(stderr, " | mflo glob->reg[%d] = %x", rd, glob->lo);
    break;
  case MTHI:
    fprintf(stderr, "mthi r%d", rs);
    fprintf(stderr, " | mthi glob->hi = %x", glob->reg[rs]);
    break;
  case MTLO:
    fprintf(stderr, "mtlo r%d", rs);
    fprintf(stderr, " | mtlo glob->lo = %x", glob->reg[rs]);
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
  uint8_t rs = ((*instru) >> 21) & 0x1F;
  uint8_t rt = ((*instru) >> 16) & 0x1F;
  uint16_t imm = (*instru) & 0xFFFF;
  fprintf(stderr, "[my_mips] Executing pc = 0x%08x: 0x%08x: ", glob->pc,
          *instru);
  if (opcode == 1)
  {
    switch (rt)
    {
    case BGEZ:
      fprintf(stderr, "bgez r%d, %x\n", rs, imm);
      fprintf(stderr, " | bgez glob->reg[%d] = %x, off = %x", rs, glob->reg[rs], imm);
      break;
    case BGEZAL:
      fprintf(stderr, "bgezal r%d, %x\n", rs, imm);
      fprintf(stderr, " | bgezal glob->reg[%d] = %x, off = %x", rs, glob->reg[rs], imm);
      break;
    case BLTZ:
      fprintf(stderr, "bltz r%d, %x\n", rs, imm);
      fprintf(stderr, " | bltz glob->reg[%d] = %x, off = %x", rs, glob->reg[rs], imm);
      break;
    case BLTZAL:
      fprintf(stderr, "bltzal r%d, %x\n", rs, imm);
      fprintf(stderr, " | bltzal glob->reg[%d] = %x, off = %x", rs, glob->reg[rs], imm);
      break;
    }
  }
  switch (opcode)
  {
  case ADDI:
    fprintf(stderr, "addi r%d, r%d, %x", rt, rs, imm);
    fprintf(stderr, " | addi glob->reg[%d] = %x + %x", rt, glob->reg[rs], imm);
    break;
  case ADDIU:
    fprintf(stderr, "addiu r%d, r%d, %x", rt, rs, imm);
    fprintf(stderr, " | addiu glob->reg[%d] = %x + %x", rt, glob->reg[rs], imm);
    break;
  case ANDI:
    fprintf(stderr, "andi r%d, r%d, %x", rt, rs, imm);
    fprintf(stderr, " | andi glob->reg[%d] = %x ANDI %x", rt, glob->reg[rs], imm);
    break;
  case ORI:
    fprintf(stderr, "ori r%d, r%d, %x", rt, rs, imm);
    fprintf(stderr, " | ori glob->reg[%d] = %x ORI %x", rt, glob->reg[rs], imm);
    break;
  case XORI:
    fprintf(stderr, "xori r%d, r%d, %x", rt, rs, imm);
    fprintf(stderr, " | xori glob->reg[%d] = %x XORI %x", rt, glob->reg[rs], imm);
    break;
  case SLTI:
    fprintf(stderr, "slti r%d, r%d, %x", rt, rs, imm);
    fprintf(stderr, " | slti glob->reg[%d] = %x < %x", rt, glob->reg[rs], imm);
    break;
  case SLTIU:
    fprintf(stderr, "sltiu r%d, r%d, %x", rt, rs, imm);
    fprintf(stderr, " | sltiu glob->reg[%d] = %x < %x", rt, glob->reg[rs], imm);
    break;
  case BEQ:
    fprintf(stderr, "beq r%d, r%d, %x", rs, rt, imm);
    fprintf(stderr, " | beq glob->reg[%d] = %x == %x, off = %x", rs, glob->reg[rs], glob->reg[rt], imm);
    break;
  case BNE:
    fprintf(stderr, "bne r%d, r%d, %x", rs, rt, imm);
    fprintf(stderr, " | bne glob->reg[%d] = %x != %x, off = %x", rs, glob->reg[rs], glob->reg[rt], imm);
    break;
  case BGTZ:
    fprintf(stderr, "bgtz r%d, %x", rs, imm);
    fprintf(stderr, " | bgtz glob->reg[%d] = %x > 0, off = %x", rs, glob->reg[rs], imm);
    break;
  case BLEZ:
    fprintf(stderr, "blez r%d, %x", rs, imm);
    fprintf(stderr, " | blez glob->reg[%d] = %x <= 0, off = %x", rs, glob->reg[rs], imm);
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
    fprintf(stderr, "lw r%d, %x(r%d)", rt, imm, rs);
    fprintf(stderr, " | lw glob->reg[%d] = %x", rt, ((uint32_t*)glob->memory)[(glob->reg[rs] + imm) / 4]);
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
    fprintf(stderr, "sw r%d, %x(r%d)", rt, imm, rs);
    fprintf(stderr, " | sw %x = %x", ((uint32_t*)glob->memory) + ((glob->reg[rs] + imm) / 4), glob->reg[rt]);
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

  fprintf(stderr, "[my_mips] Executing pc = 0x%08x: 0x%08x: ", glob->pc,
          *instru);
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
  fprintf(stderr, "[my_mips] Executing pc = 0x%08x: 0x%08x: syscall (%s:%d)\n",
          glob->pc, *instru, filename, line);
}

void log_instr(uint32_t *instru, char *filename, int line)
{
  uint8_t opcode = (*instru) >> 26;
  if (*instru == 0x0)
    fprintf(stderr, "[my_mips] Executing pc = 0x%08x: 0x%08x: nop (%s:%d)\n", glob->pc, *instru, filename, line);
  else if (*instru == 0xc)
    log_syscall(instru, filename, line);
  else if (opcode == 0)
    log_reg_instr(instru, filename, line);
  else if (opcode == J || opcode == JAL || opcode == TRAP)
    log_jump_instr(instru, filename, line);
  else
    log_imm_instr(instru, filename, line);
}
