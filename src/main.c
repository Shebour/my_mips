#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>

#include "main.h"
#include "registers.h"

unsigned int pc = 0;
int *memory;

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    printf("Usage: %s <path to MIPS executable>\n", argv[0]);
    return 1;
  }
  int fd = open(argv[1], O_RDONLY);

  if (fd == -1)
  {
    printf("Cannot open %s\n", argv[1]);
    return 2;
  }
  uint8_t buf[8] = { 0 };
  ssize_t ret = read(fd, buf, 8);
  if (ret == -1)
  {
    printf("Cannot read %s\n", argv[1]);
    return 3;
  }
  init_reg();
  memory = (int *)calloc(0xffff, sizeof(int));
  for (int i = 0; i <8; i++)
  {
    printf("%x", buf[i]);
  }
  printf("\n");
  return 0;
}
