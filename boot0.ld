OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)
ENTRY (_start)

MEMORY {
    bootsector(rwx) : ORIGIN = 0x7c00, LENGTH = 512
}

SECTIONS {
    . = ORIGIN(bootsector);
    bootloader_sig_pos = ORIGIN(bootsector) + LENGTH(bootsector) - 2;

    .text : {
        *(.text);
    }

    .rodata : {
        *(.rodata);
    }

    ASSERT( . <= bootloader_sig_pos , "boot0's code is too large, optimize it" )

    .zeroing : {
        FILL(0x00);
    }

    .bootloader_signature :
        AT( ORIGIN(bootsector) + LENGTH(bootsector) - 2 ) {
        SHORT(0xaa55);
    }
}
