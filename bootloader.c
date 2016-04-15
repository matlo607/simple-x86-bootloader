/**********************************************
 * boot1 : stage1, loaded by stage0 at 0x1000 *
 **********************************************/

#include "common.h"
#include "cpu.h"
#include "sys.h"
#include "time.h"
#include "video.h"
#include "memory.h"
#include "io.h"
#include "string.h"
#include "graphics.h"
#include "disk.h"
#include "e820.h"
#include "equipment.h"
#include "malloc.h"
#include "shell.h"

#define keyboard_waitkeystroke()    getc()

void setup_env(void)
{
    heap_init();
    shell_init();
}

void main(void)
{
    setup_env();

    // detach printing of stage 1 from stage 0
    puts("\r\n");

    printf("Welcome in bootloader %s release v%u.%u, stage1\r\n",
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

    int11_detect_equipment(&system_info.int11_equipments);

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

    ssize_t lower_memory = 0;
    if ((lower_memory = lower_memory_properties()) < 0) {
        puts("Fail to get the size of lower memory\r\n");
        goto fatal_failure;
    }

    printf("Low memory: %dKB\r\n", lower_memory);

    upper_memory_prop_t upper_memory;
    if (!upper_memory_properties(&upper_memory)) {
        goto fatal_failure;
    }

    printf("Upper memory: %uKB [0x%x] (maximum 15MB [0x3c00])\r\n", upper_memory._1MB_to_16MB, upper_memory._1MB_to_16MB);
    printf("              %uKB [0x%x] (%uMB)\r\n", upper_memory._16MB_to_4GB, upper_memory._16MB_to_4GB, upper_memory._16MB_to_4GB / 1024);

    system_info.e820_smap_nbentries = e820_detect_mem(system_info.e820_smap, 20);
    if (system_info.e820_smap_nbentries == -1) {
        printf("Error while mapping memory with int 15h, 0xe820\r\n");
    } else {
        printf("Memory map (nb entries = %d)\r\n", system_info.e820_smap_nbentries);
        e820_display_SMAP_entries(system_info.e820_smap, system_info.e820_smap_nbentries);
    }

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

    video_setmode(VIDEO_MODE_TEXT_16C_80x25_640x200);
    while (1) {
        shell_main();
    }

fatal_failure:
    puts("Wait forever ...");
    wait_forever();

    puts("Press any key to reboot");
    keyboard_waitkeystroke();
    //reboot();
}

