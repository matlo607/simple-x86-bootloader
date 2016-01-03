;============================================================================;
;========================== USEFULL DEFINES =================================;
;============================================================================;

; BIOS interrupts
BIOS_INT_VIDEO          equ 0x10
BIOS_INT_LOWMEMORY      equ 0x12
BIOS_INT_DRIVE          equ 0x13
BIOS_INT_EXTENDEDMEMORY equ 0x15

; define usefull characters
CHAR_NULL           equ 0x0
CHAR_LF             equ 0xa
CHAR_CR             equ 0xd

;============================================================================;
;================================ UTILS =====================================;
;============================================================================;

;============================================================================;
; void prints(char* s)
; \brief Print a string to the active page.
; \param[in] s : reg %si, pointer on the string to print
prints:
    ; BIOS interrupt: 0x10
    ; service:        0x0e
    ;
    ; Internal registers :
    ; %al : character to be written
    ; %bl : color in graphic mode
    ; %bh : page number
    ; %si : current char's address

    push       ax
    push       bx

.prints_fetch_char:
    lodsb                            ; load string at %ds:%si into %al and increment %si
    cmp        al, CHAR_NULL         ; check if the end of the string is reached (? c == '\0')
    jz         .prints_end
    mov        bl, 0xff              ; color
    mov        bh, 0                 ; active page
    mov        ah, 0x0E              ; call BIOS service (0x0e, BIOS_INT_VIDEO)
    int        BIOS_INT_VIDEO
    jmp        .prints_fetch_char

.prints_end:
    pop        bx
    pop        ax
    ret

