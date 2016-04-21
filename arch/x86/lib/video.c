#include "arch/x86/video.h"

#include <stddef.h>
#include <assert.h>
#include <arch/x86/bios.h>

void video_setmode(video_mode_t mode)
{
    assert(mode <= VIDEO_MODE_GRAPHICS_256C_640x480);

#ifdef BOOTLOADER_PROTECTED_MODE_ENABLED
    regs16_t regs;

    regs.a._Rl = mode;
    regs.a._Rh = 0x00;
    int32(0x10, &regs);
#else
    x86_regs_t regs_in, regs_out;

    x86_regs_init(&regs_in);

    regs_in.A._Rl = mode;
    regs_in.A._Rh = 0x00;
    bioscall(0x10, &regs_in, &regs_out);
#endif
}

void video_getstate(video_state_t* state)
{
#ifdef BOOTLOADER_PROTECTED_MODE_ENABLED
    regs16_t regs;

    regs.a._Rh = 0x0f;
    int32(0x10, &regs);

    state->page = regs.b._Rh;
    state->mode = regs.a._Rl;
    state->char_column_nb = regs.a._Rh;
#else
    x86_regs_t regs_in, regs_out;

    x86_regs_init(&regs_in);

    regs_in.A._Rh = 0x0f;
    bioscall(0x10, &regs_in, &regs_out);

    state->page = regs_out.B._Rh;
    state->mode = regs_out.A._Rl;
    state->char_column_nb = regs_out.A._Rh;
#endif
}

void video_draw_pixel(uint16_t column, uint16_t row, uint8_t color)
{
#ifdef BOOTLOADER_PROTECTED_MODE_ENABLED
    regs16_t regs;

    regs.c._Rx = column;
    regs.d._Rx = row;
    regs.a._Rl = color;
    regs.a._Rh = 0x0c;
    int32(0x10, &regs);
#else
    x86_regs_t regs_in, regs_out;

    x86_regs_init(&regs_in);

    regs_in.C._Rx = column;
    regs_in.D._Rx = row;
    regs_in.A._Rl = color;
    regs_in.A._Rh = 0x0c;
    bioscall(0x10, &regs_in, &regs_out);
#endif
}

void video_getcursorpos(uint8_t page, cursor_info_t* info)
{
    assert(info != NULL);

#ifdef BOOTLOADER_PROTECTED_MODE_ENABLED
    regs16_t regs;

    regs.b._Rh = page;
    regs.a._Rh = 0x03;
    int32(0x10, &regs);

    info->row = regs.d._Rh;
    info->column = regs.d._Rl;
    info->top_line = regs.c._Rh;
    info->bottom_line = regs.c._Rl;
#else
    x86_regs_t regs_in, regs_out;

    x86_regs_init(&regs_in);

    regs_in.B._Rh = page;
    regs_in.A._Rh = 0x03;
    bioscall(0x10, &regs_in, &regs_out);

    info->row = regs_out.D._Rh;
    info->column = regs_out.D._Rl;
    info->top_line = regs_out.C._Rh;
    info->bottom_line = regs_out.C._Rl;
#endif
}

void video_setcursorpos(uint8_t page, uint8_t row, uint8_t column)
{
#ifdef BOOTLOADER_PROTECTED_MODE_ENABLED
    regs16_t regs;

    regs.b._Rh = page;
    regs.d._Rh = row;
    regs.d._Rl = column;
    regs.a._Rh = 0x02;
    int32(0x10, &regs);
#else
    x86_regs_t regs_in, regs_out;

    x86_regs_init(&regs_in);

    regs_in.B._Rh = page;
    regs_in.D._Rh = row;
    regs_in.D._Rl = column;
    regs_in.A._Rh = 0x02;
    bioscall(0x10, &regs_in, &regs_out);
#endif
}
