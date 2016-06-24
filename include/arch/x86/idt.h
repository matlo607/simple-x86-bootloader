#ifndef _IDT_H
#define _IDT_H

#include <stdint.h>

#define IDT_DESC_PRIVILEGE_RING_0 0
#define IDT_DESC_PRIVILEGE_RING_1 1
#define IDT_DESC_PRIVILEGE_RING_2 2
#define IDT_DESC_PRIVILEGE_RING_3 3

#define IDT_DESC_ABSENT_IN_MEMORY  0
#define IDT_DESC_PRESENT_IN_MEMORY 1

#define IDT_DESC_TYPE_32BIT_CALL_GATE      0xC
#define IDT_DESC_TYPE_16BIT_INTERRUPT_GATE 0x6
#define IDT_DESC_TYPE_32BIT_INTERRUPT_GATE 0xE
#define IDT_DESC_TYPE_16BIT_TRAP_GATE      0x7
#define IDT_DESC_TYPE_32BIT_TRAP_GATE      0xF

typedef struct {
    uint16_t zeroed_0_7 : 8;

    uint16_t gate_type : 4;

    uint16_t zeroed_12 : 1;

    // 00 : highest privilege, 11: lowest privilege
    uint16_t privilege_level : 2;
    // 0: absent, 1: segment present in memory
    uint16_t present : 1;
} __attribute__ ((packed)) idt_descriptor_flags_t;

/* IDT's interrupt descriptor */
typedef struct idt_descriptor_s {

    uint16_t offset_0_15;

    uint16_t segment_selector;

    idt_descriptor_flags_t flags;

    uint16_t offset_16_31;

} __attribute__ ((packed)) idt_descriptor_t;

/* Registre IDTR */
typedef struct idtr_s {
        uint16_t limit;
        uint32_t base;
} __attribute__ ((packed)) idtr_t;

typedef void (*i86_irq_handler_t)(void);

void i86_idt_init(void);
void i86_idt_install_ir(uint8_t int_n, i86_irq_handler_t handler, idt_descriptor_flags_t flags);

#define IDT32_NB_DESCRIPTORS 256

// Exceptions
// ----------
#define DIV0INT             0x00
#define SINGLESTEPINT       0x01
#define NMIINT              0x02
#define BREAKPOINTINT       0x03
#define OVERFLOWINT         0x04
#define BOUNDSCHECKINT      0x05
#define INVALIDOPCODEINT    0x06
#define DEVAVAILIBILITYINT  0x07
#define DOUBLEFAULTINT      0x08
#define INVALIDTSSINT       0x0a
#define NOSEGMENTINT        0x0b
#define STACKINT            0x0c
#define GENPROTECTINT       0x0d
#define PAGEINT             0x0e
#define FPUINT              0x10
#define ALIGNCHECKINT       0x11
#define MACHINECHECKINT     0x12
#define SIMDFPUINT          0x13

#define IDT32_EXCEPTIONS_INT_VECT_OFFSET 0x0

#define IDT32_EXCEPTIONS_INT_OFFSET(intno) \
    (IDT32_EXCEPTIONS_INT_VECT_OFFSET + (intno))

// hardware (PIC) interrupts
// -------------------------
#define CLOCKINT    0
#define KEYBOARDINT 1

#define PIC1INTNBR 8
#define PIC2INTNBR 8
#define HARDWAREINTNBR (PIC1INTNBR + PIC2INTNBR)

#define IDT32_PIC1_INT_VECT_OFFSET 0x20
#define IDT32_PIC2_INT_VECT_OFFSET 0x70

#define IDT32_HARDWARE_INT_OFFSET(intno) \
    (((intno) >= PIC1INTNBR) ? IDT32_PIC2_INT_VECT_OFFSET + (intno) : IDT32_PIC1_INT_VECT_OFFSET + (intno))

// software interrupts
// -------------------
#define TIMERINT    0
#define KBDINPUTINT 1

#define IDT32_SOFTWARE_INT_VECT_OFFSET 0x80

#define IDT32_SOFTWARE_INT_OFFSET(intno) \
    (IDT32_SOFTWARE_INT_VECT_OFFSET + (intno))

#endif
