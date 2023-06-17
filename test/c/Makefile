OBJS = crt.o stdlib.o my_program.o
PROGRAM = my_program

PROGRAM_ELF = $(PROGRAM).elf

PREFIX		= mipsel-elf-
CC			= $(PREFIX)gcc
COPY		= $(PREFIX)objcopy

COPYFLAGS	= -O binary

DEBUGFLAGS	= -g
CFLAGS		= \
	$(DEBUGFLAGS) \
	-mips1 -msoft-float \
	-nostdlib -fno-pic -mno-abicalls
LDFLAGS		= -T program.lds -Wl,--build-id=none

all: $(PROGRAM)
PHONY: clean distclean

$(PROGRAM_ELF): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)
$(PROGRAM): $(PROGRAM_ELF)
	$(COPY) $(COPYFLAGS) $< $@

%.o: %.s
	$(CC) $(CFLAGS) -o $@ -c $<
%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(OBJS)
distclean: clean
	rm -f $(PROGRAM) $(PROGRAM_ELF)
