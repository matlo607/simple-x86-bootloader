OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)
ENTRY (_start)

MEMORY {
    bootsector(rwx) : ORIGIN = 0x7c00, LENGTH = 512
}

SECTIONS {
    . = ORIGIN(bootsector);

    .text : {
        *(.text);
    }

    .rodata : {
        *(.rodata*);
    }

    .bss : {
        *(.bss*)
    }

    .data : {
        *(.data*)
    }

    .zeroing : {
        FILL(0x00);
    }

    .bootloader_signature :
        AT( ORIGIN(bootsector) + LENGTH(bootsector) - 2 ) {
        SHORT(0xaa55);
    }

    bootloader_max_len = ORIGIN(bootsector)
                         + LENGTH(bootsector)
                         - SIZEOF(.bootloader_signature);

    ASSERT( . <= bootloader_max_len, "boot0's code is too large, optimize it" )
}
