#include "arch/x86/int12.h"

#include <arch/x86/bios.h>

int16_t int12_detect_lower_mem(void)
{
    int16_t lower_mem = 0;

#ifdef BOOTLOADER_PROTECTED_MODE_ENABLED
    regs16_t regs;

    int32(0x12, &regs);

    if (regs.eflags.CF) {
        lower_mem = -1;
    } else {
        // number of contiguous 1kB blocks of memory before EBDA
        lower_mem = regs.a._Rx;
    }
#else
    x86_regs_t regs_in, regs_out;

    x86_regs_init(&regs_in);

    bioscall(0x12, &regs_in, &regs_out);

    if (regs_out.eflags.CF) {
        lower_mem = -1;
    } else {
        // number of contiguous 1kB blocks of memory before EBDA
        lower_mem = regs_out.A._Rx;
    }
#endif

    // available memory = lower_mem * 1024 - 0x500
    return lower_mem;
}
