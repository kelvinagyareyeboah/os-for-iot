ENTRY(_start)

SECTIONS
{
    . = 0x8000;
    
    .text : {
        KEEP(*(.text.boot))
        *(.text)
    }

    .rodata : {
        *(.rodata)
    }

    .data : {
        *(.data)
    }

    .bss : {
        . = ALIGN(16);
        __bss_start = .;
        *(.bss)
        *(COMMON)
        __bss_end = .;
    }
}
