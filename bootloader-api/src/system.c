#include "system.h"

#ifdef DEBUG

#include "screen.h"
#include "keyboard.h"

void assert(bool cond, const char* error_msg)
{
    if (!cond) {
        printf("assertion failure: %s\r\n", error_msg);
        //halt_cpu();

        prints("Press any key to reboot");
        //keyboard_waitkeystroke();
        keyboard_getc(false);
        reboot();
    }
}

#endif
