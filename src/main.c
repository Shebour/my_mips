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

#define MEM_SIZE 0x10

void *sp = NULL;

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
  if (st.st_size >= MEM_SIZE)
  {
    perror("Cannot load program in memory : not enough memory\n");
    return NULL;
  }
  printf("File size: %jd bytes\n", st.st_size);
  void *memory_file =
      mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);

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

  void *memory = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANON, -1, 0);
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

  for (size_t i = 0; i < (MEM_SIZE / sizeof(uint32_t)); i++)
  {
    uint32_t *value = ((uint32_t *)memory) + i;
    printf("%p : %08x\n", memory + i * sizeof(uint32_t), *value);
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

  void *memory = init_memory(argv[1]);
  if (!memory)
    return 1;
  sp = memory + MEM_SIZE - sizeof(uint32_t);
  printf("sp = %p\n", sp);

  if (munmap(memory, MEM_SIZE) == -1)
  {
    perror("Error unmapping file\n");
    return 1;
  }

  init_reg();
  return 0;
}
