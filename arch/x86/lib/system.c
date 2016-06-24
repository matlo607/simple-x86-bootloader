#include "sys/system.h"

#include <arch/x86/cpu.h>
#include <arch/x86/bios.h>
#include <string.h>

void reboot(void)
{
#ifdef BOOTLOADER_PROTECTED_MODE_ENABLED
    regs16_t regs;
    memset(&regs, 0, sizeof(regs16_t));

    int32(0x19, &regs);
#else
    __asm__ __volatile__ ( "int $0x19" );
#endif
}

/**
 * \brief Disable interrupts permanently and halt the CPU until
 * the user performs a manual reboot.
 **/
void hang_forever(void)
{
    // disable interrupts
    cli();

    while (1) {
        // set the cpu in stand-by
        hlt();
    }
}
