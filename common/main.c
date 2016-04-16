/**********************************************
 * boot1 : stage1, loaded by stage0 at 0x1000 *
 **********************************************/

#include <stdio.h>
#include <stdlib.h>

#include <shell.h>
#include <sys/system.h>
#include <sys/setup.h>
#include <version.h>

// to remove from here and create an interface to them because
// this is specific to i386
#include <graphics.h>
#include <video.h>
#include <disk.h>
#include <cpu.h>

#define keyboard_waitkeystroke()    getc()

void setup(void)
{
    platform_specific_startup();
    platform_startup();
    shell_init();
}

void main(void)
{
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

    // detach printing of stage 1 from stage 0
    puts("\r\n");

    printf("Welcome in bootloader %s release v%u.%u, stage1\r\n",
            BOOTLOADER_RELEASE_MODE,
            BOOTLOADER_VERSION,
            BOOTLOADER_REVISION);

    setup();

    keyboard_waitkeystroke();
    video_setmode(VIDEO_MODE_TEXT_16C_80x25_640x200);

    puts("Reset disk controller: ");
    if (disk_reset(boot_drive_nb)) {
        puts("succeeded\r\n");
    } else {
        puts("failed\r\n");
        goto fatal_failure;
    }

    puts("read boot0 ...\r\n");
    uint8_t* sector_buffer = malloc(512 * sizeof(uint8_t));

    uint16_t reg_ds = 0;
    reg_get_data_segment(&reg_ds);
    //__asm__ __volatile__("movw %%ds, %[val];" : [val] "=g" (reg_ds) : : );

    uint8_t nb_sectors_to_read = 1;
    uint8_t nb_read_sectors = disk_read_sectors(boot_drive_nb, reg_ds, (uint16_t)((uint32_t)sector_buffer), 0, 0, 1, nb_sectors_to_read);
    if (nb_read_sectors != nb_sectors_to_read) {
        printf("disk_read_sectors error : read %u sectors instead of %u\r\n", nb_read_sectors, nb_sectors_to_read);
    }

    // get boot signature
    uint16_t* p_signature = (uint16_t*) (sector_buffer + 0x01fe);
    printf("boot signature = 0x%x\r\n", *p_signature);

    //video_setmode(VIDEO_MODE_TEXT_16C_80x25_640x200);

    while (1) {
        shell_main();
    }

fatal_failure:

    //puts("Wait forever ...");
    //hang_forever();

    puts("Press any key to reboot");
    keyboard_waitkeystroke();
    reboot();
}

