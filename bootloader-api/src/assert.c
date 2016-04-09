#include "assert.h"

#include "io.h"
#include "sys.h"

#ifdef DEBUG

void assert(bool cond, const char* error_msg)
{
    if (!cond) {
        printf("assertion failure: %s\r\n", error_msg);

        puts("Press any key to reboot");
        getc();

        reboot();
    }
}

#else

void assert(bool cond, const char* error_msg)
{
    (void) cond;
    (void) error_msg;
}

#endif
