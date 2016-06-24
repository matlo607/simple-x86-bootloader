#ifndef _CPU_MODE_SWITCHING_H
#define _CPU_MODE_SWITCHING_H

#include <arch/x86/gdt.h>
#include <arch/x86/idt.h>

typedef struct {
    gdtr_t     ptr_gdt;
    idtr_t     ptr_idt;
    x86_regs_t regs;
} __attribute__ ((packed)) cpu_switch_mode_context_t;

extern cpu_switch_mode_context_t cntxt_p_mode_16bit;
extern cpu_switch_mode_context_t cntxt_p_mode_32bit;

#endif
