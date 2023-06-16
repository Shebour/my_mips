#ifndef REGISTERS_H
#define REGISTERS_H

#include <stdint.h>

#define NB_REG 32

extern uint32_t registers[NB_REG];

void init_reg();

#endif /* REGISTERS_H */
