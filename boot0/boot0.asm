;============================================================================;
; boot0.asm: stage0, load and initialize stage1                              ;
;============================================================================;

;============================================================================;
[bits 16] ; we are working with 16-bits instructions
;[org 0x7c00] ; set the offset of all referenced addresses to 0x7c00

;============================================================================;
;================================== DEFINES =================================;
;============================================================================;

%include "DEF.INC"

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
; Stage 1's location on disk
BOOT1_HEAD0_CYLINDER0_SECTOR_BEGIN  equ 2      ; the first sector is used by boot0
BOOT1_HEAD0_CYLINDER0_NB_SECTORS    equ 17     ; 8.5kB
BOOT1_HEAD0_CYLINDER1_SECTOR_BEGIN  equ 1
BOOT1_HEAD0_CYLINDER1_NB_SECTORS    equ 18     ; 9kB


;============================================================================;
;================================= CODE =====================================;
;============================================================================;

section .text
    global _start
_start:
    jmp        main

;==================
; OS name (8 bytes)
;==================
os_name db 'mattOS', CHAR_NULL, CHAR_NULL

;====================
; OEM parameter block
;====================
bpbBytesPerSector:      DW 512
bpbSectorsPerCluster:   DB 1
bpbReservedSectors:     DW 1
bpbNumberOfFATs:        DB 2
bpbRootEntries:         DW 224
bpbTotalSectors:        DW 2880
bpbMedia:               DB 0xF0
bpbSectorsPerFAT:       DW 9
bpbSectorsPerTrack:     DW 18
bpbHeadsPerCylinder:    DW 2
bpbHiddenSectors:       DD 0
bpbTotalSectorsBig:     DD 0
bsDriveNumber:          DB 0
bsUnused:               DB 0
bsExtBootSignature:     DB 0x29
bsSerialNumber:         DD 0xa0a1a2a3
bsVolumeLabel:          DB "MOS FLOPPY "
bsFileSystem:           DB "FAT12   "

%include "UTIL.INC"
;============================================================================;

main:

registers_initialization:
    cli                              ; disable interrupts while changing registers

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

    ; As the init values of data segment, extra data segment and stack segment are 0,
    ; we don't use the defined values above (DATA_SEGMENT, EXTRA_DATA_SEGMENT,
    ; STACK_SEGMENT).

    ; load segment registers
    ; %DS : data segment for the bootloader (rodata, data, bss and stack)
    ; %ES : extra data segment used to load the second stage bootloader
    mov        ax, 0
    ;mov        ax, DATA_SEGMENT
    mov        ds, ax
    ;mov        ax, EXTRA_DATA_SEGMENT
    mov        es, ax

    ; prepare stack segment
    ;mov        ax, STACK_SEGMENT
    mov        ss, ax
    mov        sp, STACK_BASE_OFFSET ; set the stack pointer to the top of the stack

    sti                              ; re-enable interrupts

    ;mov        si, msg_init_registers
    ;call       prints

reset_drive:
    ; Reset disk drive
    ; The drive number is contained in %dl (initialized by the BIOS),
    ; do not touch it, just give it in argument to the drive's reset
    ; BIOS service.
    mov        cl, 5                 ; maximum number of attempts = 5
.call_reset_service:
    ;clc                              ; clear CF (carry flag)
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
    ;mov        si, msg_reset_drive_success
    ;call       prints

read_boot1_from_drive:
    ; Read 'boot1' from the hard disk drive.
    ; 'boot0' is located at (C,H,S)=(0,0,1). 'boot1' follows at (0,0,2) until (1,0,18).
    ; 'boot1' will be stored at EXTRA_DATA_SEGMENT:BOOT1_START_ADDR and will occupy 17.5kB.
    ;
    ; Details about the BIOS service :
    ; - the drive number in %bl stays untouched.
    ; - the head number is stored in %dh.
    ; - the 10-bits track/cylinder number is stored in 2 registers, %cl (bits 9,8 of
    ;   the track number on bits 7,6) and %ch (bits 7,0 of the track number on bits
    ;   7,0).
    ; - the 6-bits sector number is stored in %cl (bits 5,0).
    ;
    ; %al <-- 17,18
    ; %dl <-- drive_number
    ; %dh <-- 0
    ; %bx <-- @buffer
    ; %ch <-- 0,1
    ; %cl <-- 2,1
    mov        dh, 0                        ; head number = 0
.read_C0_H0_S2_to_S18:
    mov        al, BOOT1_HEAD0_CYLINDER0_NB_SECTORS
    mov        bx, BOOT1_START_ADDR ; address of buffer --> EXTRA_DATA_SEGMENT:new_offset_sector
    mov        cx, 0x0002           ; track number = 0, sector number = 2
    jmp        .read_bios_call

.read_C1_H0_S1_to_S18:
    mov        al, BOOT1_HEAD0_CYLINDER1_NB_SECTORS
    mov        bx, (BOOT1_START_ADDR + 512*17) ; address of buffer --> EXTRA_DATA_SEGMENT:new_offset_sector
    mov        cx, 0x0101           ; track number = 1, sector number = 1

.read_bios_call:
    mov        ah, 0x02             ; call BIOS service (0x02, BIOS_INT_DRIVE)
    ;clc                            ; clear CF (carry flag)
    int        BIOS_INT_DRIVE
    jnc        .read_succeeded              ; if an error occurs, CF(carry flag) is set to 1
                                            ; CF set to 0, all the sectors were read correctly
.read_fail:
    cmp        cx, 0x0002
    je         .read_C0_H0_S2_to_S18
    jmp        .read_C1_H0_S1_to_S18

.read_succeeded:
    cmp        cx, 0x0002
    je         .read_C1_H0_S1_to_S18

    ;mov        si, msg_read_boot1_success
    ;call       prints
    jmp        DWORD EXTRA_DATA_SEGMENT:BOOT1_START_ADDR

fatal_error:
    mov        si, msg_please_reboot
    call       prints
    cli                              ; disable interrupts permanently
    hlt                              ; halt the CPU until the user performs a manual reboot


;============================================================================;
;================================== DATA ====================================;
;============================================================================;
section .rodata
;    msg_init_registers  db 'Bootloader v0.1, stage 0', CHAR_CR, CHAR_LF, \
;                           'Initialization of registers %cs, %ds, %es, %ss, %sp [OK]', \
;                           CHAR_CR, CHAR_LF, CHAR_NULL
;                           ;'Canonicalization (%cs:%ip) [OK]', CHAR_CR, CHAR_LF, \
;                           ;'Data segment %ds at 0000:7c00', CHAR_CR, CHAR_LF, \
;                           ;'Extended data segment %es at 0000:1000', CHAR_CR, CHAR_LF, \
;                           ;'Stack size 1kB, %ss:%sp at 0000:7000', CHAR_CR, CHAR_LF, \
;
;    msg_reset_drive_success db 'Reset drive controller [OK]', CHAR_CR, CHAR_LF, CHAR_NULL
     msg_reset_drive_failure db 'Fail to reset drive controller', CHAR_NULL
;
;    msg_read_boot1_success  db 'Copy boot1 from drive [OK]', CHAR_CR, CHAR_LF, \
;                               'Jump to stage 1 at 0000:1000', CHAR_CR, CHAR_LF, CHAR_NULL
     msg_read_boot1_failure  db 'Fail to read boot1 from drive', CHAR_NULL
     msg_please_reboot       db ', please reboot', CHAR_NULL



%define __SWAP_16__(n)  (((n) << 8) | ((n) >> 8))

;====================
; MBR Partition table
;====================

%define BOOTABLE_YES 0x80
%define BOOTABLE_NO  0x0

%define ID_FAT12 0x01

; first partition table entry at offset 0x1be
section .partition1
    %define P1_START_HEAD     0
    %define P1_START_SECTOR   2
    %define P1_START_CYLINDER 0

    %define P1_END_HEAD       0
    %define P1_END_SECTOR     18
    %define P1_END_CYLINDER   79

    p1_boot_indicator:               db BOOTABLE_YES
    p1_starting_head:                db P1_START_HEAD
    p1_starting_sector_and_cylinder: dw ((P1_START_CYLINDER << 6) | P1_START_SECTOR)
    p1_system_id:                    db ID_FAT12
    p1_ending_head:                  db P1_END_HEAD
    p1_ending_sector_and_cylinder:   dw ((P1_END_CYLINDER << 6) | P1_END_SECTOR)
    p1_relative_sector:              dd 0 ; for now, we cannot fill this field
    p1_total_sectors:                dd 1440 ; = 80 * 18

section .partition2
    times 16 db 0 ; unused partition

section .partition3
    times 16 db 0 ; unused partition

section .partition4
    times 16 db 0 ; unused partition

;    times 510-($-$$)    db CHAR_NULL
;
;    ; MBR signature
;    dw 0xAA55
