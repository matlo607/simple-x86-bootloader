;============================================================================;
; boot0.asm: stage0, load and initialize stage1                              ;
;============================================================================;

;============================================================================;
[bits 16] ; we are working with 16-bits instructions
[org 0x7c00] ; set the offset of all referenced addresses to 0x7c00
;============================================================================;

;============================================================================;
;========================== USEFULL DEFINES =================================;
;============================================================================;

; BIOS interrupts
BIOS_INT_VIDEO      equ 0x10
BIOS_INT_DRIVE      equ 0x13

; define usefull characters
CHAR_NULL           equ 0x0
CHAR_LF             equ 0xa
CHAR_CR             equ 0xd

; Code segment (CODE_SEGMENT:CODE_OFFSET) = (0000:7c00)
; The code offset is provided by the absolute long jump
CODE_SEGMENT        equ 0x0000

; Data segment
DATA_SEGMENT        equ 0x0000
EXTRA_DATA_SEGMENT  equ 0x0000

; Stack segment (STACK_SEGMENT:STACK_BASE_OFFSET) = (0008:1000) --> (0008:0000)
STACK_SEGMENT       equ 0x8000
STACK_BASE_OFFSET   equ 0x1000 ; 4kB

;============================================================================;
;================================= CODE =====================================;
;============================================================================;

    ; disable interrupts
    cli

    ; canonicalize the instruction pointer
    ;-------------------------------------
    ; The BIOS loads the MBR and begins its execution at the address 0x7c00.
    ; This adress can be reprensented in real mode as 0000:7c00 (%cs:%ip).
    ; However some BIOSes actually begin the execution at the adress 07c0:0000.
    ; To deal with this discrepancy, we have to canonicalize %cs:%ip to a
    ; known code_segment:offset pair. For this purpose, we use an absolute
    ; long jump as follows.
    jmp        CODE_SEGMENT:next_line_as_code_segment_offset
next_line_as_code_segment_offset:

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
    mov        bp, sp                ; set the base pointer equal to the top of the stack

    ; enable interrupts
    sti

    mov        si, hellomsg
    call       prints

    mov        si, canonicalizationmsg
    call       prints

    mov        si, segmentsmsg
    call       prints

    mov        si, stackmsg
    call       prints

    ; Reset disk drive
    ; The drive number is contained in %dl (initialized by the BIOS),
    ; do not touch it, just give it in argument to the drive's reset
    ; procedure.
.reset_drive:
    mov        cl, 5                        ; maximum number of attempts if recoverable failure = 5
    call       reset_drive_system           ; drive number in %dl, not modified since stage 0 began
    cmp        ah, 0x00
    je         .read_boot1
    ; an error occured, it is impossible to continue booting
    mov        si, msg_reset_drive_failure
    call       prints
    jmp        .fatal_error

    ; Read boot1 from the hard disk drive
.read_boot1:
    mov        si, msg_reset_drive_success
    call       prints

.fatal_error:
    call halt                               ; halt bootloader execution

;-----------
;-- UTILS --
;-----------

;============================================================================;
; void prints(char* s)
; \brief Print a string to the active page.
; \param[in] s : reg %si, pointer on the string to print
prints:
    ; BIOS interrupt: 0x10
    ; service:        0x0e
    ; effect:         write teletype to active page
    ;
    ; Internal registers :
    ; %al : character to be written
    ; %bl : foreground color in graphic mode
    ; %si : current char's address

    ; save context
    push       ax
    push       bx

.prints_fetch_char:
    lodsb                            ; load string at %ds:%si into %al and increment %si
    cmp        al, CHAR_NULL         ; check if the end of the string is reached (? c == '\0')
    jz         .prints_end
    mov        bl, 0x07              ; foreground color
    mov        ah, 0x0E              ; call BIOS service (0x0e, BIOS_INT_VIDEO)
    int        BIOS_INT_VIDEO
    jmp        .prints_fetch_char

.prints_end:
    ; restore context
    pop        bx
    pop        ax
    ret


;============================================================================;
; void halt(void)
; \brief Sends the processor into a permanent halted status.
; \note  The only way out of this is to manually reboot.
halt:
    hlt                              ; halt the system
    jmp        halt                  ; just in case an interrupt is risen to loop indefinitely


;============================================================================;
; int reset_drive_system(uint8_t drive_nb, uint8_t max_nb_attempts)
; \brief Reset the drive system given in argument.
; \param[in] drive_nb :         reg %dl, drive number
; \param[in] max_nb_attempts :  reg %cl, maximum number of attempts if failure before abandonment
; \return 0x00 if successfull
;         BIOS's status of operation otherwise
; \note returned value is stored in %ah
reset_drive_system:
    push       cx
.reset_drive:
    mov        ah, 0x00              ; call BIOS service (0x00, BIOS_INT_DRIVE)
    int        BIOS_INT_DRIVE
    jnc        .exit                 ; if an error occurs, CF(carry flag) is set to 1
    cmp        cx, 0                 ; check if we still can attempt a new reset
    je         .exit
    dec        cx
    jmp        .reset_drive

.exit:
    pop        cx
    ret


;============================================================================;
;================================== DATA ====================================;
;============================================================================;
    hellomsg            db 'Bootloader v0.1, stage 0', CHAR_CR, CHAR_LF, CHAR_NULL

    canonicalizationmsg db 'Canonicalization (%cs:%ip) at 0000:7c00 [OK]', CHAR_CR, CHAR_LF, CHAR_NULL
    segmentsmsg         db 'Data segment %ds at 0000:7c00 [OK]', CHAR_CR, CHAR_LF, \
                           'Extended data segment %es at 0000:7c00 [OK]', CHAR_CR, CHAR_LF, \
                           CHAR_NULL
    stackmsg            db 'Stack segment %ss at 8000:0000', CHAR_CR, CHAR_LF, \
                           'Stack pointer %sp at 8000:1000', CHAR_CR, CHAR_LF, \
                           'Base pointer %bp at 8000:1000', CHAR_CR, CHAR_LF, \
                           'Stack size: 1kB', CHAR_CR, CHAR_LF, \
                           CHAR_NULL

    msg_reset_drive_success db 'Reset drive controller [OK]', CHAR_CR, CHAR_LF, CHAR_NULL
    msg_reset_drive_failure db 'Reset drive controller: fatal failure, please reboot', CHAR_NULL

    ; NULL character stuffing
    times 510-($-$$)    db CHAR_NULL

    ; MBR signature
    dw 0xAA55
