#include "sys/setup.h"

#include <video.h>
#include <equipment.h>
#include <e801.h>
#include <e820.h>
#include <disk.h>

#include <stdio.h>

#define keyboard_waitkeystroke()    getc()

bool platform_specific_startup(void)
{
    bool status = true;

    if (boot_drive_nb & 0x80) {
        printf("Boot from disk (0x%x)\r\n", boot_drive_nb);
    } else {
        printf("Boot from floppy (0x%x)\r\n", boot_drive_nb);
    }

    equipment_info_t int11_equipments;

    int11_detect_equipment(&int11_equipments);

    video_state_t video_state;
    video_getstate(&video_state);

    printf("Video state :\r\n" \
            "-------------\r\n" \
            "Mode: 0x%x\r\n" \
            "Active page: %d\r\n"\
            "Number of character columns: %d\r\n\r\n",
            video_state.mode, video_state.page, video_state.char_column_nb);

    keyboard_waitkeystroke();

    // memory detection

    ssize_t lower_memory = 0;
    if ((lower_memory = lower_memory_properties()) < 0) {
        puts("Fail to get the size of lower memory\r\n");
        status = false;
    } else {
        printf("Low memory: %dKB\r\n", lower_memory);
    }

    keyboard_waitkeystroke();

    upper_memory_prop_t upper_memory;
    upper_memory_properties(&upper_memory);

    printf("Upper memory: %uKB [0x%x] (maximum 15MB [0x3c00])\r\n", upper_memory._1MB_to_16MB, upper_memory._1MB_to_16MB);
    printf("              %uKB [0x%x] (%uMB)\r\n", upper_memory._16MB_to_4GB, upper_memory._16MB_to_4GB, upper_memory._16MB_to_4GB / 1024);

    keyboard_waitkeystroke();

    SMAP_entry_t e820_smap[20];
    ssize_t e820_smap_nbentries = e820_detect_mem(e820_smap, 20);

    if (e820_smap_nbentries == -1) {
        printf("Error while mapping memory with int 15h, 0xe820\r\n");
    } else {
        printf("Memory map (nb entries = %d)\r\n", e820_smap_nbentries);
        e820_display_SMAP_entries(e820_smap, e820_smap_nbentries);
    }

    return status;
}

