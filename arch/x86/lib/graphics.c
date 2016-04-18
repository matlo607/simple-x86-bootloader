#include "arch/x86/graphics.h"

#include <arch/x86/video.h>

void graphics_draw_line(int16_t A_x, int16_t A_y, int16_t B_x, int16_t B_y, uint8_t color)
{
    int16_t l_y;
    int16_t coef_h = B_y - A_y;
    int16_t coef_l = B_x - A_x;

    if (coef_l == 0) {
        uint16_t start_y, end_y;

        if (A_y < B_y) {
            start_y = A_y;
            end_y = B_y;
        } else {
            start_y = B_y;
            end_y = A_y;
        }

        for (int16_t l_y = start_y; l_y <= end_y; ++l_y) {
            video_draw_pixel(A_x, l_y, color);
        }

    } else if (coef_h == 0) {
        int16_t start_x, end_x;

        if (A_x < B_x) {
            start_x = A_x;
            end_x = B_x;
        } else {
            start_x = B_x;
            end_x = A_x;
        }

        for (int16_t l_x = start_x; l_x <= end_x; ++l_x) {
            video_draw_pixel(l_x, A_y, color);
        }

    } else {
        int16_t start_x, end_x, start_y;

        if (A_x < B_x) {
            start_x = A_x;
            end_x = B_x;
            start_y = A_y;

        } else {
            start_x = B_x;
            end_x = A_x;
            start_y = B_y;

            coef_h = -coef_h;
            coef_l = -coef_l;
        }


        for (int16_t l_x = start_x; l_x <= end_x; ++l_x) {
            l_y = (l_x * coef_h + start_y * coef_l) / coef_l;
            video_draw_pixel(l_x, l_y, color);
        }
    }
}

void graphics_draw_rectangle(int16_t A_x, int16_t A_y, int16_t C_x, int16_t C_y, uint8_t color, bool full)
{
    int16_t inc;

    for (int16_t y = A_y; y <= C_y; ++y) {
        if (!full && y > A_y && y < C_y) {
            inc = C_y - A_y;
        } else {
            inc = 1;
        }
        for (int16_t x = A_x; x <= C_x; x += inc) {
            video_draw_pixel(x, y, color);
        }
    }
}

