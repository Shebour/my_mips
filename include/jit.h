#ifndef JIT_H
#define JIT_H

/* JIT-accelerated execution loop. Compiles runs of simple ALU instructions to
 * native x86-64 and falls back to the interpreter for everything else. Returns
 * the same codes as execute(): 0 on clean stop, 1 on error. */
int execute_jit(void);

#endif /* JIT_H */
