#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "define.h"
#include "instructions.h"
#include "utils.h"

struct global *glob = NULL;

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    fprintf(stderr, "Usage: %s <path to MIPS executable>\n", argv[0]);
    return 1;
  }

  if (!init_global(argv[1]))
    return 1;

  printf("sp : %08x\n", glob->reg[SP]);
  printf("code size = %lu bytes\n", glob->code_size);
  /*
  for (size_t i = 0; i < (MEM_SIZE / sizeof(uint32_t)); i++)
  {
    uint32_t *value = ((uint32_t *) glob->memory) + i;
    printf("%p : %08x\n", glob->memory + i * sizeof(uint32_t), *value);
  }*/
  parse_inst();

  if (munmap(glob->memory, MEM_SIZE) == -1)
  {
    perror("Error unmapping file\n");
    return 1;
  }
  free(glob);
  return 0;
}
