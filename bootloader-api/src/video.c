#include "video.h"
#include "system.h"

video_mode_t video_setmode(video_mode_t mode)
{
    video_mode_t new_mode = 0;

    assert(mode <= VIDEO_MODE_TEXT_MONO_80x25/*VIDEO_MODE_GRAPHICS_4-16C_80x25_640x350*/,
            "specified mode does not exist");

    __asm__ __volatile__ (
            "int $0x10" : "=a" (new_mode) : "a"(0x0000 | mode)
            );

    return new_mode;
}

void video_getcursorpos(u8 page, cursor_info_t* info)
{
    if (info) {
        __asm__ __volatile__ (
                "movb %[bios_service_get_cursor_info], %%ah;"
                "movb %[page], %%bh;"
                "int $0x10;"
                "movb %%dh, %[row];"
                "movb %%dl, %[column];"
                "movb %%ch, %[top_line];"
                "movb %%cl, %[bottom_line];"
                : [row] "=g" (info->row),
                  [column] "=g" (info->column),
                  [top_line] "=g" (info->top_line),
                  [bottom_line] "=g" (info->bottom_line)
                : [bios_service_get_cursor_info] "i" (0x03),
                  [page] "g" (page)
                );
    }
}

void video_setcursorpos(u8 page, u8 row, u8 column)
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
                          );
}

