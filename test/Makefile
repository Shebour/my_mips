# my_mips

AS		= mipsel-elf-as
LD		= mipsel-elf-ld
COPY		= mipsel-elf-objcopy

ASFLAGS		= -mips32 -msoft-float --em=mipslelf
LDFLAGS		= -T c/program.lds --build-id=none -EL
COPYFLAGS	= -O binary --only-section=.text

SRC		= hello_world.s \
		  add.s \
			sub.s \
		  lui.s \
		  read_int.s \
		  read_string.s
OBJ		= $(SRC:.s=.o)
ELF		= $(OBJ:.o=.elf)
EXE		= $(OBJ:.o=.exe)

SUPPORT_SRC	= c/crt.s \
		  c/stdlib.s
SUPPORT_OBJ	= $(SUPPORT_SRC:.s=.o)

.PHONY: all clean distclean

all: $(ELF) $(EXE)

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

%.elf: %.o $(SUPPORT_OBJ)
	$(LD) $(LDFLAGS) $(SUPPORT_OBJ) $< -o $@

%.exe: %.elf
	$(COPY) $(COPYFLAGS) $< $@

clean:
	rm -f $(EXE) $(ELF)
