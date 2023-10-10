#ifndef LOGGER_H
#define LOGGER_H

#include <string.h>

#include "cpu.h"

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

void log_reg_instr(uint32_t *instru, char *filename, int line);
void log_imm_instr(uint32_t *instru, char *filename, int line);
void log_jump_instr(uint32_t *instru, char *filename, int line);
void log_syscall(uint32_t *instru, char *filename, int line);

#endif /* LOGGER_H */
