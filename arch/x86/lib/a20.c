#include "arch/x86/a20.h"

#include <stddef.h>
#include <arch/x86/bios.h>

int a20gate_enable(void)
{
    int status = EXIT_SUCCESS;

#ifdef BOOTLOADER_PROTECTED_MODE_ENABLED
    regs16_t regs;

    regs.a._Rl = 0x1;
    regs.a._Rh = 0x24;

    int32(0x15, &regs);

    if (regs.eflags.CF) {
        status = EXIT_FAILURE;
    }
#else
    x86_regs_t regs_in, regs_out;

    x86_regs_init(&regs_in);

    regs_in.D._Rl = 0x1;
    regs_in.A._Rh = 0x24;

    bioscall(0x15, &regs_in, &regs_out);

    if (regs_out.eflags.CF) {
        status = EXIT_FAILURE;
    }
#endif

    return status;
}
