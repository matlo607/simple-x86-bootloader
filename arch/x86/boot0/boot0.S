.code16gcc

#include "arch/x86/config_mem.h"

#define CHAR_NULL 0x0
#define CHAR_LF   0xa
#define CHAR_CR   0xd

    .section .rodata
msg_please_reboot:
    .asciz "An error was encountered, please reboot"

    .section .bss
    .lcomm boot_drv , 1

    .section .data
#ifdef BOOTLOADER_PROTECTED_MODE_ENABLED
gdt:
    .byte 0, 0, 0, 0, 0, 0, 0, 0
gdt_cs: /* flat model */
    .byte 0xFF, 0xFF, 0x0, 0x0, 0x0, 0b10011011, 0b11011111, 0x0
gdt_ds: /* flat model */
    .byte 0xFF, 0xFF, 0x0, 0x0, 0x0, 0b10010011, 0b11011111, 0x0

    .set gdt_size,      (. - gdt)
    .set gdt_cs_offset, (gdt_cs - gdt)
    .set gdt_ds_offset, (gdt_ds - gdt)

gdtptr:
    .hword 0  /* limite */
    .word 0 /* base */
#endif

.section .text
    .global _start

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

    jmpl $BOOT0_CODE_SEGMENT,$next_line
next_line:

    movw $BOOT0_DATA_SEGMENT, %ax
    movw %ax, %ds
    movw $BOOT0_EXTRA_DATA_SEGMENT, %ax
    movw %ax, %es

    movb %dl, (boot_drv)

    movw $BOOT0_STACK_SEGMENT, %ax
    movw %ax, %ss
    movw $BOOT0_STACK_BASE_OFFSET, %sp

    sti

drive_reset:
    movb $0x00, %ah
    int $0x13
    jc fatal_error

drive_read_boot:
    movb $BOOT1P1_HEAD, %dh
    movb $BOOT1P1_NB_SECTORS, %al
    movw $BOOT1P1_START_ADDR_RM, %bx
    movw $BOOT1P1_START_SECTOR, %cx
    movb $0x02, %ah
    int $0x13
    jc fatal_error

    movb $BOOT1P2_HEAD, %dh
    movb $BOOT1P2_NB_SECTORS, %al
    movw $BOOT1P2_START_ADDR_RM, %bx
    movw $BOOT1P2_START_SECTOR, %cx
    movb $0x02, %ah
    int $0x13
    jc fatal_error


#ifndef BOOTLOADER_PROTECTED_MODE_ENABLED

    jmpl $BOOT1_CODE_SEGMENT_RM,$BOOT1_START_ADDR_RM

#else

a20gate_enable:
    movb $0x1, %al
    movb $0x24, %ah
    int $0x15
    jc fatal_error

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
    movl $STACK_BASE_OFFSET, %esp

    jmpl $gdt_cs_offset,$BOOT1_START_ADDR_PM

#endif

fatal_error:
    movw $msg_please_reboot, %si
    call prints
    cli
    hlt
