#include "sys/system.h"

#include <stdio.h>
#include <stdarg.h>

static void panic_finish(void)
{
    hang_forever();
}

void panic(const char *format, ...)
{
    va_list args_list;

    va_start(args_list, format);

    printf(format, args_list);

    va_end(args_list);

    panic_finish();
}
