#ifndef STDLIB_H_
#  define STDLIB_H_

void print_int(int i);
void print_string(const char *s);
int read_int(void);
void read_string(char *buffer, int size);
void* sbrk(int increment);
void exit(void);
void print_char(char c);
char read_char(void);
int open(const char* file, int flags, int mode);
int read(int fd, char* buf, int count);
int write(int fd, char* buf, int count);
void close(int fd);
void exit2(int res);

#endif /* STDLIB_H_ */
