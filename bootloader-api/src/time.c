#include "time.h"

#include "cpu.h"

void wait_forever(void)
{
    interrupts_disable();
    halt_cpu();
}

