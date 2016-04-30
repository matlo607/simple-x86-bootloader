#include <arch/x86/config_mem.h>

#ifndef BOOTLOADER_PROTECTED_MODE_ENABLED
    .code16gcc
#endif

    .section .bss

    .global boot_drive_nb
boot_drive_nb:
    .byte 1

    .section .text

/* main function address */
.extern main
#ifdef BOOTLOADER_PROTECTED_MODE_ENABLED
.extern gdt_init
#endif

/*------------------------------*/
/* defined in the linker script */
/*------------------------------*/

/* address of the beginning of data sections */
.extern data_sections_size
/* size of the data sections */
.extern data_sections_beginning


    .global _start
_start:

#ifdef BOOTLOADER_PROTECTED_MODE_ENABLED
    /* We expect that boot0 has loaded the gdt so that we are in flat protected mode */

    /* relocate data sections to data segment (DATA_SEGMENT:DATA_BASE_OFFSET) */

    movl $data_sections_size, %ebx
    addl $DATA_BASE_OFFSET, %ebx
    movl $data_sections_beginning, %esi
    movl $DATA_BASE_OFFSET, %edi
0:
    movl (%esi), %eax
    movl %eax, (%edi)
    addl $4, %esi
    addl $4, %edi
    cmp %ebx, %edi
    jge 1f
    jmp 0b

1:
    /* save the drive number */
    movl $boot_drive_nb, %ebx
    movb %dl, (%ebx)

#else /* BOOTLOADER_REAL_MODE */
    /* Prepare the segments to perform the relocation */
    cli
    movw $SEGMENT_DATA, %ax
    movw %ax, %ds
    movw $SEGMENT_EXTRA_DATA, %ax
    movw %ax, %es

    movw $SEGMENT_STACK, %ax
    movw %ax, %ss
    movw $STACK_BASE_OFFSET, %sp
    sti

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
#endif /* BOOTLOADER_REAL_MODE */

    /* long jump to set correctly the %CS register */
#ifdef BOOTLOADER_PROTECTED_MODE_ENABLED
    /* init GDT for real/protected mode switch */
    call gdt_init
    call main
#else
    jmpl $SEGMENT_CODE,$main
#endif

    cli
hang:
    hlt
    jmp hang
