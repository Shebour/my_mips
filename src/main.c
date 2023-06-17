#include "main.h"

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "registers.h"
#define BUF_SIZE 4

unsigned int pc = 0;
int *memory;

int read_file(char *path)
{
    uint8_t buf[BUF_SIZE] = { 0 };

    int fd = open(path, O_RDONLY);

    if (fd == -1)
    {
        printf("Cannot open %s\n", path);
        return 2;
    }

    ssize_t ret = read(fd, buf, BUF_SIZE);
    if (ret == -1)
    {
        printf("Cannot read %s\n", path);
        return 3;
    }
    while (ret != 0)
    {
        for (int i = 0; i < BUF_SIZE; i++)
        {
            printf("%x", buf[i]);
            buf[i] = 0x0;
        }
        printf("\n");
        ret = read(fd, buf, BUF_SIZE);
    }

    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: %s <path to MIPS executable>\n", argv[0]);
        return 1;
    }
    read_file(argv[1]);
    init_reg();
    memory = (int *)calloc(0xffff, sizeof(int));
    return 0;
}
