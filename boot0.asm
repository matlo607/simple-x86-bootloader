;============================================================================;
; boot0.asm: stage0, load and initialize stage1                              ;
;============================================================================;

;============================================================================;
[bits 16] ; we are working with 16-bits instructions
[org 0x7c00] ; set the offset of all referenced addresses to 0x7c00

jmp start

%include "UTIL.INC"
;============================================================================;

;============================================================================;
;================================== DEFINES =================================;
;============================================================================;

; Code segment (CODE_SEGMENT:CODE_OFFSET) = (0000:7c00)
; The code offset is provided by the absolute long jump
CODE_SEGMENT        equ 0x0000

; Data segments
DATA_SEGMENT        equ 0x0000
EXTRA_DATA_SEGMENT  equ 0x0000

; Stack segment (STACK_SEGMENT:STACK_BASE_OFFSET) = (0000:7000) --> (0000:6000)
STACK_SEGMENT       equ 0x0000
STACK_BASE_OFFSET   equ 0x7000 ; stack size = 4kB

; Stage 1's location in RAM
BOOT1_START_ADDR    equ 0x1000
BOOT1_NB_SECTORS    equ 17     ; 8.5kB

;============================================================================;
;================================= CODE =====================================;
;============================================================================;

start:
    ; disable interrupts
    cli

registers_initialization:

    ; canonicalize the instruction pointer
    ;-------------------------------------
    ; The BIOS loads the MBR and begins its execution at the address 0x7c00.
    ; This adress can be reprensented in real mode as 0000:7c00 (%cs:%ip).
    ; However some BIOSes actually begin the execution at the adress 07c0:0000.
    ; To deal with this discrepancy, we have to canonicalize %cs:%ip to a
    ; known code_segment:offset pair. For this purpose, we use an absolute
    ; long jump as follows.
    jmp        DWORD CODE_SEGMENT:.next_line
.next_line:

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

    ; enable interrupts
    sti

    mov        si, msg_init_registers
    call       prints

reset_drive:
    ; Reset disk drive
    ; The drive number is contained in %dl (initialized by the BIOS),
    ; do not touch it, just give it in argument to the drive's reset
    ; BIOS service.
    mov        cl, 5                 ; maximum number of attempts = 5
.call_reset_service:
    mov        ah, 0x00              ; call BIOS service (0x00, BIOS_INT_DRIVE)
    int        BIOS_INT_DRIVE
    jnc        .reset_succeeded      ; if an error occurs, CF(carry flag) is set to 1
    cmp        cl, 0                 ; check if we still can attempt a new reset
    je         .reset_failed         ; counter_attempt == 0, drive's reset failed too many times, abandon
    dec        cl                    ; decrement the possible attempts' counter
    jmp        .call_reset_service

.reset_failed:
    ; an error occured, it is impossible to continue booting
    mov        si, msg_reset_drive_failure
    call       prints
    jmp        fatal_error

.reset_succeeded:
    mov        si, msg_reset_drive_success
    call       prints

read_boot1_from_drive:
    ; Read 'boot1' from the hard disk drive.
    ; 'boot0' is located at (C,H,S)=(0,0,1). 'boot1' follows at (0,0,2) until (0,0,18).
    ; 'boot1' will be stored at EXTRA_DATA_SEGMENT:BOOT1_START_ADDR and will occupy 8.5kB.
    ;
    ; Details about the BIOS service :
    ; - the drive number in %bl stays untouched.
    ; - the head number is stored in %dh and will not change to read the 17 sectors.
    ; - the 10-bits track/cylinder number is stored in 2 registers, %cl (bits 9,8 of
    ;   the track number on bits 7,6) and %ch (bits 7,0 of the track number on bits
    ;   7,0). This number will not change to read the 17 sectors.
    ; - the 6-bits sector number is stored in %cl (bits 5,0).
    push       BOOT1_START_ADDR             ; address of buffer
                                            ; at the beginning--> EXTRA_DATA_SEGMENT:BOOT1_START_ADDR
    push       BOOT1_NB_SECTORS             ; number of sectors to read = 17
    push       2                            ; start reading at (C,H,S)=(0,0,2)
    push       5                            ; maximum number of attempts = 5
    mov        bp, sp                       ; update base pointer to address the stored values in stack
    mov        dh, 0                        ; head number = 0
.call_read_service:
    mov        al, [ss:bp+4]                ; get the number of remaining sectors to read
    xor        cx, cx                       ; track number = 0, sector number = 0
    mov        cl, [ss:bp+2]                ; start reading at (C,H,S)=(0,0,last_sector_unread)
    mov        bx, [ss:bp+6]                ; address of buffer --> EXTRA_DATA_SEGMENT:new_offset_sector
    mov        ah, 0x02                     ; call BIOS service (0x02, BIOS_INT_DRIVE)
    int        BIOS_INT_DRIVE

    jnc        .read_succeeded              ; if an error occurs, CF(carry flag) is set to 1
                                            ; CF set to 0, all the sectors were read correctly
    ; if sectors were read, check the number in %al
    cmp        DWORD [ss:bp], 0             ; check if we still can attempt a new read
    je         .read_failed                 ; counter_attempt == 0, read failed too many times, abandon
    dec        DWORD [ss:bp]                ; decrement the possible attempts' counter
    sub        [ss:bp+4], al                ; update the number of sectors to read
    add        [ss:bp+2], al                ; update the last sector read
    mov        ah, 0
    imul       ax, 512
    add        [ss:bp+6], ax                ; update the buffer's address
    jmp        .call_read_service

.read_failed:
    mov        si, msg_read_boot1_failure
    call       prints
    jmp        fatal_error

.read_succeeded:
    add        sp, 6
    mov        si, msg_read_boot1_success
    call       prints
    jmp        DWORD EXTRA_DATA_SEGMENT:BOOT1_START_ADDR

fatal_error:
    ; Loop waiting a manual reboot ('hlt' instruction prevents a CPU usage of 100%)
    hlt                              ; halt the CPU until the next external interrupt is fired
    jmp        fatal_error           ; just in case an interrupt is risen, jump again to loop indefinitely


;============================================================================;
;================================== DATA ====================================;
;============================================================================;
    msg_init_registers  db 'Bootloader v0.1, stage 0', CHAR_CR, CHAR_LF, \
                           'Initialization of registers %cs, %ds, %es, %ss, %sp [OK]', \
                           CHAR_CR, CHAR_LF, CHAR_NULL
                           ;'Canonicalization (%cs:%ip) [OK]', CHAR_CR, CHAR_LF, \
                           ;'Data segment %ds at 0000:7c00', CHAR_CR, CHAR_LF, \
                           ;'Extended data segment %es at 0000:1000', CHAR_CR, CHAR_LF, \
                           ;'Stack size 1kB, %ss:%sp at 0000:7000', CHAR_CR, CHAR_LF, \

    msg_reset_drive_success db 'Reset drive controller [OK]', CHAR_CR, CHAR_LF, CHAR_NULL
    msg_reset_drive_failure db 'Fail to reset drive controller, please reboot', CHAR_NULL

    msg_read_boot1_success  db 'Copy boot1 from drive [OK]', CHAR_CR, CHAR_LF, \
                               'Jump to stage 1 at 0000:1000', CHAR_CR, CHAR_LF, CHAR_NULL
    msg_read_boot1_failure  db 'Fail to read boot1 from drive, please reboot', CHAR_NULL

    ; NULL character stuffing
    times 510-($-$$)    db CHAR_NULL

    ; MBR signature
    dw 0xAA55
