#ifndef DISASM_H
#define DISASM_H

#include <stddef.h>
#include <stdint.h>

/* Write the assembly mnemonic of one instruction into out (e.g.
 * "addiu r29, r29, -40"). This is a pure function of the instruction
 * word; it is shared by the -l trace logger and the integrated debugger. */
void disassemble(uint32_t instr, char *out, size_t n);

#endif /* DISASM_H */
