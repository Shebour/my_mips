#include "map_elf.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "logger.h"
#include "utils.h"

extern struct global *glob;

int map_section(Elf32_Phdr prg_header)
{
  void *sec_load = mmap(NULL, prg_header.p_memsz, PROT_WRITE | PROT_READ, MAP_ANON | MAP_PRIVATE, -1, 0);
  if (sec_load == MAP_FAILED)
  {
    LOG_ERROR("Error mapping PT_LOAD section: %s", strerror(errno));
    return 0;
  }

  void *memory = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
  if (memory == MAP_FAILED)
    return 0;

  return 1;
}

void get_program_size(void)
{
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
    close(fd);
    return 0;
  }

  for (size_t i = 0; i < glob->elf_header.e_phnum; i++)
  {
    if (glob->prg_header[i].p_type == PT_LOAD)
    {
      Elf32_Phdr prg_header = glob->prg_header[i];

    }
  }
  close(fd);
  return 1;
}
