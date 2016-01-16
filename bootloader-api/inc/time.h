#ifndef _BIOS_TIME
#define _BIOS_TIME

#include "common.h"

/**
 * \brief Disable interrupts permanently and halt the CPU until
 * the user performs a manual reboot.
 **/
extern void wait_forever(void);

#endif
