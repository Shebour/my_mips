#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <stddef.h>
#include <stdint.h>

void parse_register(uint32_t *instruction);
void parse_immediate(uint32_t *instruction);
void parse_jump(uint32_t *instruction);
void parse_inst();

#endif /* INSTRUCTIONS_H */
