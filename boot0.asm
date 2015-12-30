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

    push       hellomsg
    push       0x07
    call       prints

    push       canonicalizationmsg
    push       0x03
    call       prints

    push       segmentsmsg
    push       0x30
    call       prints

    push       stackmsg
    push       0x55
    call       prints

    call halt                        ; halt bootloader here for now before going further



;-----------
;-- UTILS --
;-----------

; void prints(char* s, uint8_t color)
; \brief Print a string to the active page.
; \param[in] s      pointer on the string to print
; \param[in] color  color to use (only the lower byte is considered)
prints:
    ; BIOS interrupt: 0x10
    ; service:        0x0e
    ; effect:         write teletype to active page
    ;
    ; Internal registers :
    ; %AL : character to be written
    ; %BL : foreground color in graphic mode

    ; save context
    push       bp
    mov        bp, sp
    push       ax
    push       bx

    mov        bl, [ss:bp+4]         ; get the color
    mov        si, [ss:bp+6]         ; get the string address

.prints_fetch_char:
    lodsb                            ; load string at %ds:%si into %al and increment %si
    cmp        al, CHAR_NULL         ; check if the end of the string is reached (? c == '\0')
    jz         .prints_end
    mov        ah, 0x0E              ; call BIOS service (0x0e, int 0x10)
    int        BIOS_INT_VIDEO
    jmp        .prints_fetch_char

.prints_end:
    ; restore context
    pop        bx
    pop        ax
    pop        bp
    ret


; void halt(void)
; \brief Sends the processor into a permanent halted status.
; \note  The only way out of this is to manually reboot.
halt:
    hlt                              ; halt the system
    jmp        halt


    ; some NOP stuffing
    times 20            db 0x90


;============================================================================;
;================================== DATA ====================================;
;============================================================================;
    hellomsg            db 'Executing bootloader v0.1', CHAR_CR, CHAR_LF, CHAR_NULL
    canonicalizationmsg db 'Canonicalization (%cs:%ip) at 0000:7c00 [OK]', CHAR_CR, CHAR_LF, CHAR_NULL
    segmentsmsg         db 'Data segment %ds at 0000:7c00 [OK]', CHAR_CR, CHAR_LF, \
                           'Extended data segment %es at 0000:7c00 [OK]', CHAR_CR, CHAR_LF, \
                           CHAR_NULL
    stackmsg            db 'Stack segment %ss at 8000:0000', CHAR_CR, CHAR_LF, \
                           'Stack pointer %sp at 8000:1000', CHAR_CR, CHAR_LF, \
                           'Base pointer %bp at 8000:1000', CHAR_CR, CHAR_LF, \
                           'Stack size: 1kB', CHAR_CR, CHAR_LF, \
                           CHAR_NULL

    ; NULL character stuffing
    times 510-($-$$)    db CHAR_NULL

    ; MBR signature
    dw 0xAA55
