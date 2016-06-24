#include "arch/x86/idt.h"
#include <arch/x86/gdt.h>
#include <arch/x86/cpu_mode_switching.h>
#include <arch/x86/config_mem.h>

#include <debug.h>
#include <string.h>
#include <stdio.h>
#include <arch/x86/cpu.h>

#define ISR32_SEG (GDT32_INDEX_CODE_SEG * sizeof(gdt_descriptor_t))

static idt_descriptor_t IDT32[IDT32_NB_DESCRIPTORS];

// handler's assembly wrapper
extern void _asm_default_int(void);
extern void _asm_default_pic1_int(void);
extern void _asm_default_pic2_int(void);

static void idt_desc_init(uint16_t select, i86_irq_handler_t handler, idt_descriptor_flags_t flags, idt_descriptor_t *desc);

void i86_idt_init(void)
{
    memset(IDT32, 0, IDT32_NB_DESCRIPTORS * sizeof(idt_descriptor_t));

    // all interrupts are initialized as interrupt gates and only can be executed
    // with ring 0's priviledges.
    idt_descriptor_flags_t flags;
    memset(&flags, 0, sizeof(idt_descriptor_flags_t));

    flags.gate_type = IDT_DESC_TYPE_32BIT_INTERRUPT_GATE;
    flags.privilege_level = IDT_DESC_PRIVILEGE_RING_0;
    flags.present = IDT_DESC_PRESENT_IN_MEMORY;

    for (uint16_t i=0; i<IDT32_NB_DESCRIPTORS; ++i) {

        if (IDT32_PIC1_INT_VECT_OFFSET <= i
                && i < IDT32_PIC1_INT_VECT_OFFSET + PIC1INTNBR) {
            idt_desc_init(ISR32_SEG, _asm_default_pic1_int, flags, &IDT32[i]);

        } else if (IDT32_PIC1_INT_VECT_OFFSET <= i
                && i < IDT32_PIC1_INT_VECT_OFFSET + PIC1INTNBR) {
            idt_desc_init(ISR32_SEG, _asm_default_pic2_int, flags, &IDT32[i]);

        } else {
            idt_desc_init(ISR32_SEG, _asm_default_int, flags, &IDT32[i]);
        }
    }

    cntxt_p_mode_32bit.ptr_idt.base = (uint32_t) &IDT32;
    cntxt_p_mode_32bit.ptr_idt.limit = sizeof(idt_descriptor_t) * IDT32_NB_DESCRIPTORS;

    // load the 32-bit IDT's pointer
    lidt(&cntxt_p_mode_32bit.ptr_idt);
#ifdef DEBUG_IDT
    debug_printf("IDT: ridt=%#08x, base=%#08x, limit=%#x\r\n",
            &cntxt_p_mode_32bit.ptr_idt,
            cntxt_p_mode_32bit.ptr_idt.base,
            cntxt_p_mode_32bit.ptr_idt.limit);
#endif
}

void i86_idt_install_ir(uint8_t int_n, i86_irq_handler_t handler, idt_descriptor_flags_t flags)
{
    idt_desc_init(ISR32_SEG, handler, flags, &IDT32[int_n]);
}

static void idt_desc_init(uint16_t select, i86_irq_handler_t handler, idt_descriptor_flags_t flags, idt_descriptor_t *desc)
{
    desc->offset_0_15 = ((uint32_t) handler) & 0x0000ffff;
    desc->segment_selector = select;
    desc->flags = flags;
    desc->offset_16_31 = ( ((uint32_t) handler) & 0xffff0000 ) >> 16;
}

void isr_default_int(void)
{
    printf("isr_default_int : unhandled interrupt\r\n");
}

void isr_default_pic1_int(void)
{
    printf("isr_default_pic2_int : unhandled interrupt\r\n");
}

void isr_default_pic2_int(void)
{
    printf("isr_default_pic2_int : unhandled interrupt\r\n");
}
