SECTIONS
{
    . = 0x00100000;  /* Start of the kernel */

    .text : {
        *(.text)
    }

    .bss : {
        *(.bss)
    }

    . = ALIGN(0x1000); /* Align to the next page boundary */
    .stack (NOLOAD) : {
        stack_space = .;
        . = . + 4096; /* Reserve space for the stack */
    }
}
