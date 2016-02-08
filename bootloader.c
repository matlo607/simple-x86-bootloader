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
#include "graphics.h"
#include "disk.h"
#include "string.h"

#define BOOTLOADER_RELEASE_MODE "alpha"
#define BOOTLOADER_VERSION      0
#define BOOTLOADER_REVISION     1

void main(void)
{
    // detach printing of stage 1 from stage 0
    prints("\r\n");

    printf("Welcome in bootloader %s v%u.%u, stage1\r\n",
            BOOTLOADER_RELEASE_MODE,
            BOOTLOADER_VERSION,
            BOOTLOADER_REVISION);

    video_state_t video_state;
    video_getstate(&video_state);
    printf("Video state :\r\n" \
           "-------------\r\n" \
           "Mode: 0x%x\r\n" \
           "Active page: %d\r\n"\
           "Number of character columns: %d\r\n\r\n",
           video_state.mode, video_state.page, video_state.char_column_nb);

    equipment_info_t equipment = equipment_get_info();
    equipment_print_info(equipment);

    keyboard_waitkeystroke();

    // change to graphic mode
    video_setmode(VIDEO_MODE_GRAPHICS_256C_640x480);

    graphics_draw_rectangle(5, 5, 100, 100, 0x2, true);
    graphics_draw_rectangle(10, 10, 90, 90, 0x3, false);

    graphics_draw_line(5, 7, 100, 90, 0x1);
    graphics_draw_line(0, 0, 100, 90, 0x1);
    graphics_draw_line(0, 100, 90, 90, 0x1);
    graphics_draw_line(50, 0, 50, 100, 0x1);
    graphics_draw_line(0, 50, 100, 50, 0x1);
    graphics_draw_line(100, 5, 8, 90, 0x1);

    keyboard_waitkeystroke();

    // change back to text mode
    video_setmode(VIDEO_MODE_TEXT_16C_80x25_640x200);

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
    printf("              %uKB [0x%x] (%uMB)\r\n", upper_memory._16MB_to_4GB, upper_memory._16MB_to_4GB, upper_memory._16MB_to_4GB / 1024);

    prints("Reset disk controller: ");
    if (disk_reset(boot_drive_nb)) {
        prints("succeeded\r\n");
    } else {
        prints("failed\r\n");
        goto fatal_failure;
    }

    //printf("read boot0 ...\r\n");
    //uint8_t nb_read_sectors = disk_read_sectors(boot_drive_nb, 0x8000, 0x0000, 0, 0, 1);
    //printf("number of read sectors : %u\r\n", nb_read_sectors);

    //uint16_t* p_signature = (uint16_t*)0x801ff;
    //printf("boot signature = 0x%h\r\n", p_signature);

fatal_failure:
    prints("Wait forever ...");
    wait_forever();

    prints("Press any key to reboot");
    keyboard_waitkeystroke();
    //reboot();
}

