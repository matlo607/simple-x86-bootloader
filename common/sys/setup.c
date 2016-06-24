#include "sys/setup.h"

#include <sys/tty.h>
#include <sys/hal.h>

#include <version.h>
#include <stdio.h>

void platform_startup(void)
{
    tty_initialize();

    printf("Welcome in bootloader %s release v%u.%u, stage1\r\n",
            BOOTLOADER_RELEASE_MODE,
            BOOTLOADER_VERSION,
            BOOTLOADER_REVISION);

    hal_init();
}
