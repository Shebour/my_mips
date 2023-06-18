#include "main.h"

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "registers.h"

int *sp = NULL;

int *init_memory(char *path)
{
  int fd = open(path, O_RDONLY);
  if (fd < 0)
  {
    perror("Cannot open file\n");
    return NULL;
  }
  struct stat st;
  int ret = fstat(fd, &st);
  if (ret < 0)
  {
    perror("Error in fstat\n");
    return NULL;
  }
  printf("File size: %jd bytes\n", st.st_size);
  void *memory_file = (void *)mmap(NULL, st.st_size / sizeof(int), PROT_READ, MAP_PRIVATE, fd, 0);

  if (memory_file == MAP_FAILED)
  {
    perror("Error mapping file\n");
    return NULL;
  }

  if (close(fd) < 0)
  {
    perror("Error closing file: %s\n");
    return NULL;
  }

  int *memory = (int *)mmap(NULL, 0xfff, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
  if (memory == MAP_FAILED)
  {
    perror("Error mapping program memory\n");
    return NULL;
  }
  memory = memcpy(memory, memory_file, st.st_size);

  if (munmap(memory_file, st.st_size) == -1)
  {
    perror("Error unmapping file\n");
    return NULL;
  }

  for (size_t i = 0; i < (0xfff / 4); i++)
  {
    printf("%p : %08x\n", memory + i, memory[i]);
  }

  return memory;
}

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    fprintf(stderr, "Usage: %s <path to MIPS executable>\n", argv[0]);
    return 1;
  }

  int *memory = init_memory(argv[1]);
  sp = memory + 0xfff - 1;
  printf("sp = %p\n", sp);

  if (munmap(memory, 0xfff) == -1)
  {
    perror("Error unmapping file\n");
    return 1;
  }

  init_reg();
  return 0;
}
