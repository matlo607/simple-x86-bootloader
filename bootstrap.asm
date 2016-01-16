;============================================================================;
; boot1.asm: stage1, load and initialize stage2                              ;
;============================================================================;

;============================================================================;
[bits 16] ; we are working with 16-bits instructions

;============================================================================;
;================================== DEFINES =================================;
;============================================================================;

; Data segments
DATA_SEGMENT        equ 0x0000
EXTRA_DATA_SEGMENT  equ 0x0000

; Stack segment (STACK_SEGMENT:STACK_BASE_OFFSET) = (0000:7000) --> (0000:6000)
STACK_SEGMENT       equ 0x0000
STACK_BASE_OFFSET   equ 0x7000 ; stack size = 4kB

CODE_SEGMENT        equ 0x0000

;============================================================================;
;================================= CODE =====================================;
;============================================================================;

extern main

section .text
    global _start
_start:
    ;;  registers_initialization:
    cli                              ; disable interrupts while changing registers

    ; load segment registers
    ; %DS : data segment for the bootloader
    ; %ES : extra data segment for the bootloader also
    mov        ax, DATA_SEGMENT
    mov        ds, ax
    mov        ax, EXTRA_DATA_SEGMENT
    mov        es, ax

    ; prepare stack segment
    mov        ax, STACK_SEGMENT
    mov        ss, ax
    mov        sp, STACK_BASE_OFFSET ; set the stack pointer to the top of the stack

    sti                              ; re-enable interrupts

    mov        [boot_drive_nb], dl   ; save the drive number

    jmp  DWORD CODE_SEGMENT:main

;============================================================================;
;================================== DATA ====================================;
;============================================================================;
section .bss
    ; boot drive number
    global boot_drive_nb
    boot_drive_nb   resb 1
