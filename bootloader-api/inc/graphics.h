#ifndef _BIOS_GRAPHICS
#define _BIOS_GRAPHICS

#include "common.h"

extern void graphics_draw_line(int16_t A_x, int16_t A_y,
                               int16_t B_x, int16_t B_y,
                               uint8_t color);

extern void graphics_draw_rectangle(int16_t A_x, int16_t A_y,
                                    int16_t C_x, int16_t C_y,
                                    uint8_t color,
                                    bool full);

#endif
