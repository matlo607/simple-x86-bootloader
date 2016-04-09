#ifndef _EQUIPMENT_H
#define _EQUIPMENT_H

#include "common.h"

#define INITIAL_VIDEO_MODE_EGA_VGA          0x0
#define INITIAL_VIDEO_MODE_40x25_CGA        0x1
#define INITIAL_VIDEO_MODE_80x25_CGA        0x2
#define INITIAL_VIDEO_MODE_MONOCHROME       0x3

typedef struct equipment_info_s {
    uint16_t unused_bit0 : 1;
    uint16_t math_coprocessor : 1;
    uint16_t ps2_mouse : 1;
    uint16_t unused_bit3 : 1;
    uint16_t initial_video_mode : 2;
    uint16_t floppy_drives : 2;
    uint16_t unused_bit8 : 1;
    uint16_t nb_serial_adapters : 3;
    uint16_t game_adapter : 1;
    uint16_t unused_bit13 : 1;
    uint16_t nb_parallel_adapters : 2;
} equipment_info_t;

extern void int11_detect_equipment(equipment_info_t* equipment);

#endif
