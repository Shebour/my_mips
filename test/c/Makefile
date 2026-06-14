OBJS = crt.o stdlib.o my_program.o
PROGRAM = my_program

PROGRAM_ELF = $(PROGRAM).elf

# PREFIX		= mipsel-elf-
PREFIX 		= mipsel-linux-gnu-
# Ubuntu 26.04 drops the mipsel gcc cross-compiler, so compile with clang
# (native mipsel target) and link with the mipsel binutils linker directly.
CC			= clang
TARGET		= --target=mipsel-linux-gnu
LD			= $(PREFIX)ld
COPY		= $(PREFIX)objcopy

COPYFLAGS	= -O binary

DEBUGFLAGS	= -g
CFLAGS		= \
	$(TARGET) \
	$(DEBUGFLAGS) \
	-mips1 -msoft-float \
	-nostdlib -fno-pic -mno-abicalls
LDFLAGS		= -T program.lds --build-id=none -EL

all: $(PROGRAM)
PHONY: clean distclean

$(PROGRAM_ELF): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)
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
