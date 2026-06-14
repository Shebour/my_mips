#ifndef CPU_H
#define CPU_H

#include <stdint.h>

#include "decode.h"

void pc_step(int step);
void schedule_branch(uint32_t target);
uint32_t branch_dest(uint32_t i);
int execute(void);
int step_instruction(void);
int exec_inst(uint32_t *instru);
int exec_register(uint32_t *instruction);
int exec_immediate(uint32_t *instruction);
int exec_jump(uint32_t *instruction);
int exec_special2(uint32_t *instruction);

#endif /* CPU_H */
