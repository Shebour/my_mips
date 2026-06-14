#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdint.h>

/* Instruction semantics that need more than a single expression. Each takes
 * the raw instruction word and decodes its own fields (see decode.h). */
void add(uint32_t instr);
void sub(uint32_t instr);
void addi(uint32_t instr);
void mult(uint32_t instr);
void multu(uint32_t instr);
void div(uint32_t instr);
void divu(uint32_t instr);

/* Byte-addressed little-endian memory access. */
uint32_t load_byte(uint32_t address);
uint32_t load_half(uint32_t address);
uint32_t load_word(uint32_t address);
void store_byte(uint32_t address, uint32_t value);
void store_half(uint32_t address, uint32_t value);
void store_word(uint32_t address, uint32_t value);

#endif /* FUNCTIONS_H */
