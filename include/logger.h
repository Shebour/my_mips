#ifndef LOGGER_H
#define LOGGER_H

#include <string.h>

#include "cpu.h"

#define __FILENAME__                                                           \
  (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

void log_instr(uint32_t *instru, char *filename, int line);

#endif /* LOGGER_H */
