#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stdint.h>

int call_syscall(void);
void print_int(void);
void print_string(void);
void read_int(void);
void read_string(void);

#endif /* SYSCALLS_H */
