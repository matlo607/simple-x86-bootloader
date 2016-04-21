#include "sys/setup.h"

#include <sys/heap.h>
#include <sys/tty.h>

bool platform_startup(void)
{
    tty_initialize();
    heap_init();

    return true;
}
