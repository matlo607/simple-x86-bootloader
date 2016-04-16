#include <config_mem.h>

.code16gcc

.bss
    .global boot_drive_nb
boot_drive_nb:
    .byte 1

.text

/* main function address */
.extern main

    .global _start
_start:
    cli

    movw $SEGMENT_DATA, %ax
    movw %ax, %ds
    movw $SEGMENT_EXTRA_DATA, %ax
    movw %ax, %es

    movw $SEGMENT_STACK, %ax
    movw %ax, %ss
    movw $STACK_BASE_OFFSET, %sp

    sti

/*------------------------------*/
/* defined in the linker script */
/*------------------------------*/

/* address of the beginning of data sections */
.extern data_sections_size
/* size of the data sections */
.extern data_sections_beginning

    /* relocate data sections to data segment (DATA_SEGMENT:DATA_BASE_OFFSET) */
    movw $data_sections_size, %cx
    movw $data_sections_beginning, %si
    movw $DATA_BASE_OFFSET, %di
0:
    movw %cs:(%si), %ax
    movw %ax, %ds:(%di)
    addw $2, %si
    addw $2, %di
    cmp %cx, %di
    jge 1f
    jmp 0b

1:
    /* save the drive number */
    movw $boot_drive_nb, %bx
    movb %dl, (%bx)

    /* long jump to set correctly the %CS register */
    jmpl $SEGMENT_CODE,$main

