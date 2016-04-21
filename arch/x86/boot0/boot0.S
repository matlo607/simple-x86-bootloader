.code16gcc

/* #include "boot0.h" */

/* Code segment (CODE_SEGMENT:CODE_OFFSET) = (0000:7c00) */
/* The code offset is provided by the absolute long jump */
#define CODE_SEGMENT        0x0000

/* Data segments */
#define DATA_SEGMENT        0x0000
#define EXTRA_DATA_SEGMENT  0x0000

/* Stack segment (STACK_SEGMENT:STACK_BASE_OFFSET) = (0000:8000) --> (0000:7e00) // 512 bytes */
#define STACK_SEGMENT       0x0000
#define STACK_BASE_OFFSET   0x8000

/* Stage 1's location in RAM */
#define BOOT1_START_ADDR    0x1000
/* Stage 1's location on disk */
#define BOOT1_HEAD          0
#define BOOT1_NB_SECTORS    40     // 20kB

#define CHAR_NULL 0x0
#define CHAR_LF   0xa
#define CHAR_CR   0xd


    .section .rodata
msg_please_reboot:
    .asciz "An error was encountered, please reboot"

    .section .bss
boot_drv:
    .byte 1

    .section .data
gdt:
    .byte 0, 0, 0, 0, 0, 0, 0, 0
gdt_cs: /* flat model */
    .byte 0xFF, 0xFF, 0x0, 0x0, 0x0, 0b10011011, 0b11011111, 0x0
gdt_ds: /* flat model */
    .byte 0xFF, 0xFF, 0x0, 0x0, 0x0, 0b10010011, 0b11011111, 0x0

    .set gdt_size,      . - gdt
    .set gdt_cs_offset, gdt_cs - gdt
    .set gdt_ds_offset, gdt_ds - gdt

gdtptr:
    .hword 0  /* limite */
    .word 0 /* base */


.section .text
    .global _start;

_start:
    jmp init_registers

    .global prints
prints:
    push %ax
    push %bx
0:
    lodsb
    cmp $CHAR_NULL, %al
    jz 1f
    movb $0xff, %bl
    movb $0, %bh
    movb $0x0e, %ah
    int $0x10
    jmp 0b
1:
    pop %bx
    pop %ax
    ret

init_registers:
    cli

    jmpl $CODE_SEGMENT,$next_line
next_line:

    xorw %ax, %ax
    movw %ax, %ds
    movw %ax, %es

    movb %dl, (boot_drv)

    movw %ax, %ss
    movw $STACK_BASE_OFFSET, %sp

    sti

drive_reset:
    movb $0x00, %ah
    int $0x13
    jc fatal_error

drive_read_boot:
    movb $BOOT1_HEAD, %dh
    movb $BOOT1_NB_SECTORS, %al
    movw $BOOT1_START_ADDR, %bx
    movw $0x0002, %cx
    movb $0x02, %ah
    int $0x13
    jc fatal_error

#ifndef BOOTLOADER_PROTECTED_MODE_ENABLED

    jmpl $EXTRA_DATA_SEGMENT,$BOOT1_START_ADDR

#else

disable_interrupts:
    cli

gdt_init:
    movw $gdt_size, (gdtptr)
    xorl %eax, %eax
    xorl %ebx, %ebx
    movw %ds, %ax
    shl $4, %eax
    movw $gdt, %bx
    addl %ebx, %eax
    movw $gdtptr, %bx
    movl %eax, 2(%ebx)

switch_to_protected_mode:
    lgdtl (gdtptr)
    movl %cr0, %eax
    orw $0x0001, %ax
    movl %eax, %cr0

    jmp 2f
2:
    movw $gdt_ds_offset, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss
    movl $0x9F000, %esp

    jmpl $gdt_cs_offset,$BOOT1_START_ADDR

#endif

fatal_error:
    movw $msg_please_reboot, %si
    call prints
    cli
    hlt

.section .partition_table
    /* boot flag, start (H, C, S) = (0, 0, 37), Minix 1.4a, end (H, C, S) = (0, 0, 50), LBA=36, 13 sectors */
    .byte 0x80, 0x00, 0x25, 0x00, 0x80, 0x00, 0x32, 0x00, 0x24, 0x0D

    .byte 0x80, 0x00, 0x33, 0x00, 0x01, 0x00, 0x3c, 0x00, 0x32, 0x0A

    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
