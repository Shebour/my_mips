#define _POSIX_C_SOURCE 200809L
#include "map_elf.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "logger.h"
#include "utils.h"

extern struct global *glob;

void get_program_size(void)
{
  glob->code_size = 0;
  for (size_t i = 0; i < glob->elf_header.e_phnum; i++)
  {
    if (glob->prg_header[i].p_type == PT_LOAD)
      glob->code_size += glob->prg_header[i].p_memsz;
  }
}

int map_elf(int fd)
{
  get_program_size();
  if (glob->code_size >= MEM_SIZE)
  {
    LOG_ERROR("Not enough memory: available %lu", (unsigned long)MEM_SIZE);
    close(fd);
    return 0;
  }

  /* Anonymous mappings are zero-filled, which gives us the .bss for free. */
  void *memory = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANON, -1, 0);
  if (memory == MAP_FAILED)
  {
    LOG_ERROR("Error mapping program memory: %s", strerror(errno));
    close(fd);
    return 0;
  }
  glob->memory = memory;

  /* Copy every PT_LOAD segment to its virtual address. */
  for (size_t i = 0; i < glob->elf_header.e_phnum; i++)
  {
    Elf32_Phdr ph = glob->prg_header[i];
    if (ph.p_type != PT_LOAD)
      continue;
    if ((size_t)ph.p_vaddr + ph.p_filesz > MEM_SIZE)
    {
      LOG_ERROR("PT_LOAD segment out of bounds");
      close(fd);
      return 0;
    }
    if (pread(fd, (char *)memory + ph.p_vaddr, ph.p_filesz, ph.p_offset) == -1)
    {
      LOG_ERROR("Cannot read PT_LOAD segment: %s", strerror(errno));
      close(fd);
      return 0;
    }
  }

  glob->pc = glob->elf_header.e_entry;
  close(fd);
  return 1;
}
