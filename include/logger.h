#ifndef LOGGER_H
#define LOGGER_H

#include <string.h>

#include "cpu.h"

#define __FILENAME__                                                           \
  (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)


#define LOG_TRACE(...) logger(__FILENAME__, __LINE__, __VA_ARGS__)
#define LOG_DEBUG(...) logger(__FILENAME__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...) logger(__FILENAME__, __LINE__, __VA_ARGS__)
#define LOG_WARN(...) logger(__FILENAME__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) logger(__FILENAME__, __LINE__, __VA_ARGS__)

void log_instr(uint32_t *instru, char *filename, int line);

void logger(const char *file, int line, const char *fmt, ...);


#endif /* LOGGER_H */
