/**********************************************
 * boot1 : stage1, loaded by stage0 at 0x1000 *
 **********************************************/

#include "common.h"
#include "system.h"
#include "time.h"
#include "video.h"
#include "memory.h"
#include "screen.h"
#include "keyboard.h"
#include "disk.h"

#define BOOTLOADER_RELEASE_MODE "alpha"
#define BOOTLOADER_VERSION      0
#define BOOTLOADER_REVISION     1

void main(void)
{
    video_setmode(VIDEO_MODE_TEXT_16C_80x25_640x200);

    printf("Welcome in bootloader %s v%u.%u, stage1\r\n",
            BOOTLOADER_RELEASE_MODE,
            BOOTLOADER_VERSION,
            BOOTLOADER_REVISION);

    if (boot_drive_nb & 0x80) {
        printf("Boot from disk (0x%x)\r\n", boot_drive_nb);
    } else {
        printf("Boot from floppy (0x%x)\r\n", boot_drive_nb);
    }

    int16_t lower_memory = 0;
    if ((lower_memory = lower_memory_properties()) < 0) {
        prints("Fail to get the size of lower memory\r\n");
        goto fatal_failure;
    }

    printf("Low memory: %uKB\r\n", lower_memory);

    upper_memory_prop_t upper_memory;
    if (!upper_memory_properties(&upper_memory)) {
        goto fatal_failure;
    }

    printf("Upper memory: %uKB [0x%x] (maximum 15MB [0x3c00])\r\n", upper_memory._1MB_to_16MB, upper_memory._1MB_to_16MB);
    printf("              %u x 64KB [0x%x]\r\n", upper_memory._16MB_to_4GB, upper_memory._16MB_to_4GB);

    //cursor_info_t cursor;
    //video_getcursorpos(0, &cursor);
    //printf("row = %u, column = %u\r\n", cursor.row, cursor.column);
    //printc('=', 0, 10);


    prints("Reset disk controller: ");
    if (disk_reset(boot_drive_nb)) {
        prints("succeeded\r\n");
    } else {
        prints("failed\r\n");
        goto fatal_failure;
    }

    //char c;
    //while (1) {
    //    c = keyboard_getc(false);
    //    printf("char 0x%x ('%d') = %c\r\n", c, c, c);
    //}


    while (1) {
        char buffer[10];
        uint16_t count = keyboard_getline(buffer, sizeof(buffer), '\r');
        if (count >= 9) {
            buffer[9] = '\0';
        }
        printf("line = %s\r\n", buffer);
    }


fatal_failure:
    prints("Wait forever ...");
    wait_forever();

    prints("Press any key to reboot");
    //keyboard_waitkeystroke();
    keyboard_getc(false);
    //reboot();
}

