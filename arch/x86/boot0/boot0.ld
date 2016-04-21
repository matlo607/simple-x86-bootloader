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

    /* MBR partition table :
     * ---------------------
     * Each Partition Table entry is 16 bytes long, making a maximum of four
     * entries available.
     * */
    .partition_table : AT ( ORIGIN(bootsector) + 0x01BE ) { *(.partition_table*); }
    /*.partition2 : AT ( ORIGIN(bootsector) + 0x01CE ) { *(.partition2*); }
    .partition3 : AT ( ORIGIN(bootsector) + 0x01DE ) { *(.partition3*); }
    .partition4 : AT ( ORIGIN(bootsector) + 0x01EE ) { *(.partition4*); }*/

    .bootloader_signature :
        AT( ORIGIN(bootsector) + LENGTH(bootsector) - 2 ) {
        SHORT(0xaa55);
    }

    bootloader_max_len = ORIGIN(bootsector) + LENGTH(bootsector)
                         - SIZEOF(.bootloader_signature)
                         - SIZEOF(.partition_table)
                         /*- SIZEOF(.partition2)
                         - SIZEOF(.partition3)
                         - SIZEOF(.partition4)*/;

    ASSERT( . <= bootloader_max_len, "boot0's code is too large, optimize it" )
}
