# my_mips

# AS		= mipsel-elf-as
# LD		= mipsel-elf-ld
# COPY		= mipsel-elf-objcopy
AS 			= mipsel-linux-gnu-as
LD 			= mipsel-linux-gnu-ld
COPY		= mipsel-linux-gnu-objcopy


# ASFLAGS		= -mips32 -msoft-float --em=mipslelf
# hard-float so explicit COP1 (floating-point) instructions assemble
ASFLAGS 	= -mips32 -mhard-float -EL
LDFLAGS		= -T c/program.lds --build-id=none -EL
# LDFLAGS 	= -mno-abicalls -fno-pic -fno-pie -G0 -ffreestanding -EL
COPYFLAGS	= -O binary --only-section=.text

SRC		= hello_world.s \
		  add.s \
		  addu.s \
			sub.s \
		  lui.s \
		  read_int.s \
		  read_string.s \
		  logic.s \
		  shift.s \
		  arith.s \
		  hilo.s \
		  setless.s \
		  branch.s \
		  branchlink.s \
		  byte_half.s \
		  unaligned.s \
		  char.s \
		  exit2.s \
		  break.s \
		  trap.s \
		  fileio.s \
		  mips32.s \
		  errors.s \
		  badsyscall.s \
		  disasm_sampler.s \
		  fpu.s \
		  game.s \
		  bench.s
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
