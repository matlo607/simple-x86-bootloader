/**********************************************
 * boot1 : stage1, loaded by stage0 at 0x1000 *
 **********************************************/

#include "common.h"
#include "system.h"
#include "screen.h"
#include "keyboard.h"

#define BOOTLOADER_RELEASE_MODE "dummy"
#define BOOTLOADER_VERSION      1
#define BOOTLOADER_REVISION     0

void main(void)
{
    // detach printing of stage 1 from stage 0
    prints("\r\n");

    printf("Welcome in bootloader %s v%u.%u, stage1\r\n",
            BOOTLOADER_RELEASE_MODE,
            BOOTLOADER_VERSION,
            BOOTLOADER_REVISION);

    prints("Press any key to reboot");
    keyboard_waitkeystroke();

    // make space between the different boots
    prints("\r\n\r\n");

    reboot();
}

