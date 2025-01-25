#define _POSIX_C_SOURCE 200809L
#include "utils.h"

#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "logger.h"
#include "map_elf.h"

#define _DIAGASSERT(t)

extern struct global *glob;

int check_header(Elf32_Ehdr elf_header)
{
  unsigned char magic_number[5] = { 0x7F, 0x45, 0x4C, 0x46, 0};
  for (int i = 0; magic_number[i] != 0; i++)
  {
    if (magic_number[i] != elf_header.e_ident[i])
      return 0;
  }
  if (elf_header.e_ident[EI_CLASS] != ELFCLASS32)
    return 0;
  if (elf_header.e_ident[EI_DATA] != ELFDATA2LSB)
    return 0;
  if (elf_header.e_ident[EI_VERSION] != EV_CURRENT)
    return 0;
  if (elf_header.e_ident[EI_OSABI] != ELFOSABI_SYSV)
    return 0;
  if (elf_header.e_type != ET_EXEC)
    return 0;
  if (elf_header.e_machine != EM_MIPS)
    return 0;
  if (elf_header.e_version != EV_CURRENT)
    return 0;
  if (elf_header.e_phoff != 0x34)
    return 0;
  return 1;
}

int open_file(char *path)
{struct stat st;
  if (stat(path, &st) == -1)
  {
    LOG_ERROR("stat(%s) error: %s", path, strerror(errno));
    return -1;
  }

  if (!S_ISREG(st.st_mode))
  {
    LOG_ERROR("%s is not an ordinary file", path);
    return -1;
  }

  if (st.st_size >= MEM_SIZE)
  {
    LOG_ERROR("Not enough memory: available %lu", MEM_SIZE);
    return -1;
  }

  int fd = open(path, O_RDONLY);

  if (fd < 0)
  {
    LOG_ERROR("open(%s) error: %s", path, strerror(errno));
    return -1;
  }
  glob->file_size = st.st_size;
  return fd;
}

int binary_or_elf(char *path)
{
  int fd = open_file(path);
  if (fd == -1)
    return -1;
  glob->elf_file = mmap(NULL, glob->file_size, PROT_READ, MAP_PRIVATE | MAP_ANON, -1, 0);
  if (glob->elf_file == MAP_FAILED)
  {
    LOG_ERROR("Error mapping file %s: %s", path, strerror(errno));
    return -1;
  }
  Elf32_Ehdr elf_header;
  ssize_t ret = read(fd, &elf_header, 52);
  if (ret == -1 || ret != 52)
  {
    LOG_ERROR("Cannot read elf header");
    return -1;
  }
  if (check_header(elf_header))
  {
    Elf32_Phdr *prg_header = calloc(elf_header.e_phnum, elf_header.e_phentsize);
    ret = pread(fd, prg_header, elf_header.e_phnum * elf_header.e_phentsize, elf_header.e_phoff);
    if (ret == -1)
    {
      LOG_ERROR("Cannot read program header");
      return -1;
    }
    LOG_INFO("Executing ELF32 file");
    glob->elf = 1;
    glob->elf_header = elf_header;
    glob->prg_header = prg_header;
    return fd;
  }
  glob->elf = 0;
  return fd;
}

int init_memory(int fd)
{
  void *memory_file =
    mmap(NULL, glob->file_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);

  if (memory_file == MAP_FAILED)
  {
    LOG_ERROR("Error mapping file: %s", strerror(errno));
    return 0;
  }

  if (close(fd) < 0)
  {
    LOG_ERROR("Error closing file: %s", strerror(errno));
    return 0;
  }

  void *memory = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE,
      MAP_PRIVATE | MAP_ANON, -1, 0);
  if (memory == MAP_FAILED)
  {
    LOG_ERROR("Error mapping program memory: %s", strerror(errno));
    if (munmap(memory_file, glob->file_size) == -1)
      LOG_ERROR("Error unmapping file: %s", strerror(errno));
    return 0;
  }
  memory = memcpy(memory, memory_file, glob->file_size);

  if (munmap(memory_file, glob->file_size) == -1)
  {
    LOG_ERROR("Error unmapping program memory: %s", strerror(errno));
    return 0;
  }
  glob->memory = memory;
  return 1;
}

int init_global(int argc, char **argv)
{
  glob = malloc(sizeof(struct global));

  if (!glob)
    return 0;

  for (int i = 0; i < NB_REG; i++)
  {
    glob->reg[i] = 0x0;
  }
  glob->hi = 0x0;
  glob->lo = 0x0;

  glob->debug = 0;
  glob->log = 0;
  for (int i = 0; i < argc; i++)
  {
    if (argv[i][0] == '-' && (argv[i][1] != '-' || argv[i][1] != ' '))
    {
      switch (argv[i][1])
      {
      case 'd':
        glob->debug = 1;
        break;
      case 'l':
        glob->log = 1;
      default:
        break;
      }
    }
  }
  glob->pc = 0x0;
  int fd = binary_or_elf(argv[argc - 1]);
  if (glob->elf)
  {
    if (!map_elf(fd))
      return 0;
  }
  else if (!init_memory(fd))
    return 0;
  glob->reg[SP] = MEM_SIZE - sizeof(uint32_t);
  memset(glob->map, 0, sizeof(glob->map));
  glob->map_index = 0;
  return 1;
}

void clean_exit(void)
{
  if (munmap(glob->memory, MEM_SIZE) == -1)
    LOG_ERROR("Error freeing memory: %s", strerror(errno));
  if (glob->elf)
    free(glob->prg_header);
  free(glob);
}

uint32_t strtou32(const char *__restrict nptr, char **__restrict endptr,
    int base, int *rstatus)
{
  int serrno;
  uintmax_t im;
  char *ep;
  int rep;
  uint32_t lo = 0;
  uint32_t hi = UINT32_MAX;

  _DIAGASSERT(hi >= lo);

  _DIAGASSERT(nptr != NULL);

  if (endptr == NULL)
    endptr = &ep;

  if (rstatus == NULL)
    rstatus = &rep;

  serrno = errno;
  errno = 0;

  im = strtoumax(nptr, endptr, base);

  *rstatus = errno;
  errno = serrno;

  if (*rstatus == 0)
  {
    if (nptr == *endptr)
      *rstatus = ECANCELED;
    else if (**endptr != '\0')
      *rstatus = ENOTSUP;
  }

  if (im < lo)
  {
    if (*rstatus == 0)
      *rstatus = ERANGE;
    return lo;
  }
  if (im > hi)
  {
    if (*rstatus == 0)
      *rstatus = ERANGE;
    return hi;
  }

  return im;
}
