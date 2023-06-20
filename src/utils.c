#include "utils.h"

#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

extern struct global *glob;

int init_memory(char *path)
{
  int fd = open(path, O_RDONLY);
  if (fd < 0)
  {
    perror("Cannot open file\n");
    return 0;
  }
  struct stat st;
  int ret = fstat(fd, &st);
  if (ret < 0)
  {
    perror("Error in fstat\n");
    return 0;
  }
  if (st.st_size >= MEM_SIZE)
  {
    perror("Cannot load program in memory : not enough memory\n");
    return 0;
  }
  void *memory_file =
      mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);

  if (memory_file == MAP_FAILED)
  {
    perror("Error mapping file\n");
    return 0;
  }

  if (close(fd) < 0)
  {
    perror("Error closing file: %s\n");
    return 0;
  }

  void *memory = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANON, -1, 0);
  if (memory == MAP_FAILED)
  {
    perror("Error mapping program memory\n");
    return 0;
  }
  memory = memcpy(memory, memory_file, st.st_size);

  if (munmap(memory_file, st.st_size) == -1)
  {
    perror("Error unmapping file\n");
    return 0;
  }
  glob->memory = memory;
  glob->code_size = st.st_size;

  return 1;
}

int init_global(char *path)
{
  glob = malloc(sizeof(struct global));

  if (!glob)
    return 0;

  for (int i = 0; i < NB_REG; i++)
  {
    glob->reg[i] = 0x0;
  }
  glob->pc = 0x0;
  if (!init_memory(path))
    return 0;
  glob->reg[SP] = MEM_SIZE - sizeof(uint32_t);
  return 1;
}
