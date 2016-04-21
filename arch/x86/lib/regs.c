#include "arch/x86/regs.h"

#include <stdio.h>
#include <string.h>

void x86_regs_init(x86_regs_t* p_regs)
{
    memset(p_regs, 0, sizeof(x86_regs_t));

#ifndef BOOTLOADER_PROTECTED_MODE_ENABLED

    __asm__ __volatile__ (
            "movw %%ds, %[reg_ds];"
            "movw %%es, %[reg_es];"
            "movw %%fs, %[reg_fs];"
            "movw %%gs, %[reg_gs];"
            : [reg_ds] "=g" (p_regs->DS),
              [reg_es] "=g" (p_regs->ES),
              [reg_fs] "=g" (p_regs->FS),
              [reg_gs] "=g" (p_regs->GS)
            : );

#else

    p_regs->CS = 0x0000;
    p_regs->DS = 0x2000;
    p_regs->ES = 0x2000;
    p_regs->FS = 0x2000;
    p_regs->GS = 0x2000;
    p_regs->SS = 0x2000;
    p_regs->SP._R = 0xFFFC;

#endif
}

#ifdef DEBUG
void x86_regs_debug_print(const x86_regs_t* p_regs)
{
    // segment registers
    debug_printf("%%ds = %#04x\r\n", p_regs->DS);
    debug_printf("%%es = %#04x\r\n", p_regs->ES);
    debug_printf("%%fs = %#04x\r\n", p_regs->FS);
    debug_printf("%%gs = %#04x\r\n", p_regs->GS);

    debug_printf("%%eax = %#08x, %%ax = %#04x, %%ah = %#02x, %%al = %#02x\r\n",
            p_regs->A._eRx,
            p_regs->A._Rx,
            p_regs->A._Rh,
            p_regs->A._Rl);

    debug_printf("%%ebx = %#08x, %%bx = %#04x, %%bh = %#02x, %%bl = %#02x\r\n",
            p_regs->B._eRx,
            p_regs->B._Rx,
            p_regs->B._Rh,
            p_regs->B._Rl);

    debug_printf("%%ecx = %#08x, %%cx = %#04x, %%ch = %#02x, %%cl = %#02x\r\n",
            p_regs->C._eRx,
            p_regs->C._Rx,
            p_regs->C._Rh,
            p_regs->C._Rl);

    debug_printf("%%edx = %#08x, %%dx = %#04x, %%dh = %#02x, %%dl = %#02x\r\n",
            p_regs->D._eRx,
            p_regs->D._Rx,
            p_regs->D._Rh,
            p_regs->D._Rl);

    debug_printf("%%edi = %#08x, %%di = %#04x\r\n",
            p_regs->DI._eR,
            p_regs->DI._R);

    debug_printf("%%esi = %#08x, %%si = %#04x\r\n",
            p_regs->SI._eR,
            p_regs->SI._R);

    // flags
    debug_printf("eflags : %#08x\r\n", p_regs->eflags);

#if 0
    // flags' details
    debug_printf("\tCF [Carry flag] = %b\r\n", p_regs->eflags.CF);
    debug_printf("\tPF [Parity flag] = %b\r\n", p_regs->eflags.PF);
    debug_printf("\tAF [Auxiliary carry flag] = %b\r\n", p_regs->eflags.AF);
    debug_printf("\tZF [Zero flag] = %b\r\n", p_regs->eflags.ZF);
    debug_printf("\tSF [Sign flag] = %b\r\n", p_regs->eflags.SF);
    debug_printf("\tTF [Trap flag] = %b\r\n", p_regs->eflags.TF);
    debug_printf("\tIF [Interrupt flag] = %b\r\n", p_regs->eflags.IF);
    debug_printf("\tDF [Direction flag] = %b\r\n", p_regs->eflags.DF);
    debug_printf("\tOF [Overflow flag] = %b\r\n", p_regs->eflags.OF);
    debug_printf("\tIOPL [I/O Priviledge flag] = %02b\r\n", p_regs->eflags.IOPL);
    debug_printf("\tNT [Nested task flag] = %b\r\n", p_regs->eflags.NT);
    debug_printf("\tRF [Resume flag] = %b\r\n", p_regs->eflags.RF);
    debug_printf("\tVM [Virtual 8086 mode flag] = %b\r\n", p_regs->eflags.VM);
    debug_printf("\tAC [Alignment check flag] (486+) = %b\r\n", p_regs->eflags.AC);
    debug_printf("\tVIF [Virtual interrupt flag] = %b\r\n", p_regs->eflags.VIF);
    debug_printf("\tVIP [Virtual interrupt pending flag] = %b\r\n", p_regs->eflags.VIP);
    debug_printf("\tID [ID flag] = %b\r\n", p_regs->eflags.ID);
#endif
}
#endif
