#include "readelf.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "logger.h"
#include "utils.h"

int readelf(char *file)
{
  struct stat st;
  if (stat(file, &st) == -1)
  {
    fprintf(stderr, "Cannot stat %s: %s\n", file, strerror(errno));
    return 0;
  }
  if (st.st_size >= MEM_SIZE)
  {
    fprintf(stderr, "Cannot load program in memory : not enough memory\n");
    return 0;
  }
  int fd = open(file, O_RDONLY);
  if (fd == -1)
  {
    fprintf(stderr, "Cannot open %s: %s\n", file, strerror(errno));
    return 0;
  }
  void *memory_file = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (memory_file == MAP_FAILED)
  {
    fprintf(stderr, "Error mapping %s: %s\n", file, strerror(errno));
    close(fd);
    return 0;
  }
  Elf32_Ehdr elf_header;
}
