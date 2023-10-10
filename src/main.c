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
  if (argc < 2)
  {
    fprintf(stderr, "Usage: %s [-d] <path to MIPS executable>\n", argv[0]);
    return 1;
  }

  if (!init_global(argc - 1, argv + 1))
  {
    free(glob);
    return 1;
  }

  execute();

  clean_exit();
  return EXIT_SUCCESS;
}
