/**********************************************
 * boot1 : stage1, loaded by stage0 at 0x1000 *
 **********************************************/

#ifdef BOOTLOADER_MINIMAL

#include <sys/system.h>
#include <arch/x86/bios.h>

void main(void)
{
#ifdef BOOTLOADER_PROTECTED_MODE_ENABLED
    regs16_t regs;

    for (int i=0; i<10; ++i) {
        regs.a._Rl = 'M';
        regs.b._Rl = 0x07;
        regs.a._Rh = 0x0e;

        int32(0x10, &regs);
    }
#else
    x86_regs_t regs_in, regs_out;

    x86_regs_init(&regs_in);

    for (int i=0; i<10; ++i) {
        regs_in.A._Rl = 'M';
        regs_in.B._Rl = 0x07;
        regs_in.A._Rh = 0x0e;

        bioscall(0x10, &regs_in, &regs_out);
    }
#endif

    hang_forever();
}

#else

#include <stdio.h>
#include <stdlib.h>

#include <shell.h>
#include <sys/system.h>
#include <sys/setup.h>

void main(void)
{
    platform_startup();

    shell_init();

    while (1) {
        shell_main();
    }
}

#endif
