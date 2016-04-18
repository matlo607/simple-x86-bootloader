#include "sys/system.h"

#include <arch/x86/cpu.h>

void reboot(void)
{
    __asm__ __volatile__ ( "int $0x19" );
}

/**
 * \brief Disable interrupts permanently and halt the CPU until
 * the user performs a manual reboot.
 **/
void hang_forever(void)
{
    interrupts_disable();
    halt_cpu();
}
