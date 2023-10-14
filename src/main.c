#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "cpu.h"
#include "dbg.h"
#include "define.h"
#include "logger.h"
#include "readelf.h"
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
  if (glob->debug)
  {
    debug();
    clean_exit();
    return EXIT_SUCCESS;
  }
  if (execute() == 1)
    LOG_ERROR("Error during execution");

  clean_exit();
  return EXIT_SUCCESS;
}
