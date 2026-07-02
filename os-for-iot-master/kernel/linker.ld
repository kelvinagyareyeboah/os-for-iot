ENTRY(kernel_main)

SECTIONS
{
    . = 0x8000;     /* Start of kernel */

    .text : { *(.text*) }
    .rodata : { *(.rodata*) }
    .data : { *(.data*) }
    .bss : { *(.bss*) }
}

