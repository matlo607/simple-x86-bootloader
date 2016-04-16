;============================================================================;
; boot1.asm: stage1, load and initialize stage2                              ;
;============================================================================;

;============================================================================;
[bits 16] ; we are working with 16-bits instructions

;============================================================================;
;================================== DEFINES =================================;
;============================================================================;

; Data segments
DATA_SEGMENT        equ 0x1000
DATA_BASE_OFFSET    equ 0x0000

EXTRA_DATA_SEGMENT  equ 0x2000

; Stack segment (STACK_SEGMENT:STACK_BASE_OFFSET) = (1000:FFFF) --> (1000:EFFF)
STACK_SEGMENT       equ 0x1000
STACK_BASE_OFFSET   equ 0xFFFF ; stack size = 4kB

CODE_SEGMENT        equ 0x0000

;============================================================================;
;================================= CODE =====================================;
;============================================================================;

;-------------------------------
; defined in the linker script :
;-------------------------------
; address of the beginning of data sections
extern data_sections_beginning
; size of the data sections
extern data_sections_size

; main function address
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

    ; relocate data sections to data segment (DATA_SEGMENT:DATA_BASE_OFFSET)
    mov        cx, data_sections_size
    mov        si, data_sections_beginning
    mov        di, DATA_BASE_OFFSET
.copy:
    mov        ax, [cs:si]
    mov        [ds:di], ax
    add        si, 2
    add        di, 2
    cmp        di, cx
    jge        .end_copy
    jmp        .copy

.end_copy:
    ; save the drive number
    mov        [boot_drive_nb], dl

    ; long jump to set correctly the %CS register
    jmp  DWORD CODE_SEGMENT:main

;============================================================================;
;================================== DATA ====================================;
;============================================================================;
section .bss
    ; boot drive number
    global boot_drive_nb
    boot_drive_nb   resb 1
