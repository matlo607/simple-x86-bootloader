#include "equipment.h"

#include <stdio.h>
#include <assert.h>

static equipment_info_t equipment_get_info(void);
static void equipment_print_info(equipment_info_t equipement);

equipment_info_t equipment_get_info(void)
{
    equipment_info_t field = {0};

    __asm__ __volatile__ (
            "int $0x11;"
            "movw %%ax, %[equipment_info];"
            : [equipment_info] "=g" (field)
            :
            : "%ax"
            );

    return field;
}

void equipment_print_info(equipment_info_t equipment)
{
    static const char status_absent[] = "absent";
    static const char status_installed[] = "installed";
    const char* status = NULL;

    status = (equipment.math_coprocessor) ? status_installed : status_absent;
    printf("Math coprocessor: %s\r\n", status);
    status = (equipment.ps2_mouse) ? status_installed : status_absent;
    printf("PS/2 mouse: %s\r\n", status);
    status = (equipment.game_adapter) ? status_installed : status_absent;
    printf("Game adapter: %s\r\n", status);

    char* video_mode = NULL;
    switch (equipment.initial_video_mode) {
        case INITIAL_VIDEO_MODE_EGA_VGA:
            video_mode = "EGA VGA";
            break;

        case INITIAL_VIDEO_MODE_40x25_CGA:
            video_mode = "40x25 CGA";
            break;

        case INITIAL_VIDEO_MODE_80x25_CGA:
            video_mode = "80x25 CGA";
            break;

        case INITIAL_VIDEO_MODE_MONOCHROME:
            video_mode = "monochrome";
            break;

        default:
            video_mode = "unknown";
    }
    printf("Initial video mode: %s\r\n", video_mode);

    printf("Number of floppy drives: %u\r\n", equipment.floppy_drives);

    printf("Number of serial adapters: %u\r\n", equipment.nb_serial_adapters);

    printf("Number of parallel adapters: %u\r\n", equipment.nb_parallel_adapters);
}

void int11_detect_equipment(equipment_info_t* equipment)
{
    assert(equipment != NULL);

    *equipment = equipment_get_info();
    equipment_print_info(*equipment);
}
