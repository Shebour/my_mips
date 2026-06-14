#ifndef FPU_H
#define FPU_H

#include <stdint.h>

/* COP1 (floating-point) instructions, opcode 0x11: moves, arithmetic,
 * conversions, compares and bc1t/bc1f. */
int exec_cop1(uint32_t *instruction);

/* COP1 loads and stores: lwc1, ldc1, swc1, sdc1. */
int exec_cop1_ldst(uint32_t *instruction);

#endif /* FPU_H */
