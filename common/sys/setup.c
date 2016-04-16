#include "sys/setup.h"

#include <sys/heap.h>

bool platform_startup(void)
{
    heap_init();

    return true;
}
