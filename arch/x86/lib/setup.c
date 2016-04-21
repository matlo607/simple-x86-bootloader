#include "sys/setup.h"

#include <arch/x86/video.h>
#include <arch/x86/equipment.h>

#include <stdio.h>

#define keyboard_waitkeystroke()    getc()

bool platform_specific_startup(void)
{
    bool status = true;
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

    return status;
}

