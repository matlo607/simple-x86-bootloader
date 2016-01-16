#include "time.h"

#include "system.h"

void wait_forever(void)
{
    interrupts_disable();
    halt_cpu();
}

