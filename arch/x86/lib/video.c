#include "video.h"

#include <assert.h>

void video_setmode(video_mode_t mode)
{
    assert(mode <= VIDEO_MODE_GRAPHICS_256C_640x480);

    __asm__ __volatile__ (
            "movb %[bios_service_set_video_mode], %%ah;"
            "movb %[video_mode], %%al;"
            "int $0x10;"
            :
            : [bios_service_set_video_mode] "i" (0x00),
              [video_mode] "g" (mode)
            : "%eax"
            );
}

void video_getstate(video_state_t* state)
{
    __asm__ __volatile__ (
            "movb %[bios_service_get_video_state], %%ah;"
            "int $0x10;"
            "movb %%al, %[video_mode];"
            "movb %%ah, %[nb_char_columns];"
            "movb %%bh, %[active_page];"
            : [video_mode] "=m" (state->mode),
              [nb_char_columns] "=m" (state->char_column_nb),
              [active_page] "=m" (state->page)
            : [bios_service_get_video_state] "i" (0x0f)
            : "%eax", "%ebx"
            );
}

void video_draw_pixel(uint16_t column, uint16_t row, uint8_t color)
{
    __asm__ __volatile__ (
            "movb %[bios_service_draw_pixel], %%ah;"
            "movb %[color], %%al;"
            "movw %[column], %%cx;"
            "movw %[row], %%dx;"
            "int $0x10;"
            :
            : [bios_service_draw_pixel] "i" (0x0c),
              [column] "g" (column),
              [row] "g" (row),
              [color] "g" (color)
            : "%eax", "%ecx", "%edx"
            );
}

void video_getcursorpos(uint8_t page, cursor_info_t* info)
{
    if (info) {
        __asm__ __volatile__ (
                "push %%ecx;"
                "push %%edx;"
                "movb %[bios_service_get_cursor_info], %%ah;"
                "movb %[page], %%bh;"
                "int $0x10;"
                "movb %%dh, %[row];"
                "movb %%dl, %[column];"
                "movb %%ch, %[top_line];"
                "movb %%cl, %[bottom_line];"
                "pop %%edx;"
                "pop %%ecx;"
                : [row] "=m" (info->row),
                  [column] "=m" (info->column),
                  [top_line] "=m" (info->top_line),
                  [bottom_line] "=m" (info->bottom_line)
                : [bios_service_get_cursor_info] "i" (0x03),
                  [page] "g" (page)
                : "%eax", "%ebx"
                );
    }
}

void video_setcursorpos(uint8_t page, uint8_t row, uint8_t column)
{
    __asm__ __volatile__ ("movb %[bios_service_set_cursor_pos], %%ah;"
                          "movb %[page], %%bh;"
                          "movb %[row], %%dh;"
                          "movb %[column], %%dl;"
                          "int $0x10"
                          :
                          : [bios_service_set_cursor_pos] "i" (0x02),
                            [page] "g" (page),
                            [row] "g" (row),
                            [column] "g" (column)
                          : "%eax", "%ebx", "%edx"
                          );
}

