#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdint.h>

void add(uint32_t rs, uint32_t rt, uint32_t rd);
void addu(uint32_t rs, uint32_t rt, uint32_t rd);
void sub(uint32_t rs, uint32_t rt, uint32_t rd);
void subu(uint32_t rs, uint32_t rt, uint32_t rd);
void div(uint32_t rs, uint32_t rt);
void divu(uint32_t rs, uint32_t rt);
void mult(uint32_t rs, uint32_t rt);
void multu(uint32_t rs, uint32_t rt);
void slt(uint32_t rs, uint32_t rt, uint32_t rd);
void sltu(uint32_t rs, uint32_t rt, uint32_t rd);
void addi(uint32_t rs, int32_t imm, uint32_t rt);
void addiu(uint32_t rs, int32_t imm, uint32_t rt);
void slti(uint32_t rs, int32_t imm, uint32_t rt);
void sltiu(uint32_t rs, uint32_t imm, uint32_t rt);

#endif /* FUNCTIONS_H */
