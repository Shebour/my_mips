/* OUTPUT_FORMAT("elf32-mips")
OUTPUT_ARCH("mips") */
ENTRY(start)

SECTIONS
{
    . = 0x0000000 ;
    .text 0x0 :
    {
        *(.text)
    }
    .rodata :
    {
        *(.rodata)
    }
    .data :
    {
        *(.data) *(.bss) *(COMMON)
    }

    .reginfo :
    {
        *(.reginfo)
    }

    . = ALIGN(0x1000) ;
    .stack :
    {
        *(.stack)
    }

    .pdr :
    {
        *(.pdr)
    }
}
