OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)
ENTRY (_start)

MEMORY {
    boot1(rwx) : ORIGIN = 0x1000, LENGTH = 512*18
}

SECTIONS{
    . = ORIGIN(boot1);
    /*boot1_end = ORIGIN(boot1) + LENGTH(boot1);*/

    .text : {
        bootstrap.o(.text)
        *(.text*)
    }

    .rodata : {
        *(.rodata*)
    }

    .data : {
        *(.data*)
    }

    .bss : {
        *(.bss*)
    }

    /*ASSERT( . <= boot1_end , "boot1's code is too large, optimize it" )*/
}
