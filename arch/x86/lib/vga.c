#include "sys/vga.h"

#include <string.h>
#include <assert.h>
#include <arch/x86/ioport.h>

#define VGA_CRTMICRO_INDEX_REG  0x3d4
#define VGA_CRTMICRO_DATA_REG   0x3d5

#define CURSOR_LOCATION_HIGH_BYTE 0xe
#define CURSOR_LOCATION_LOW_BYTE  0xf

#define VGA_MEMORY 0xB8000
#define VGA_WIDTH  80
#define VGA_HEIGHT 25

#define vga_colorentry(fg, bg) \
    ((0x0f & (fg)) | ((bg) << 4))

#define vga_create_entry(c, color) \
    ( ((uint16_t) c) | ( ((uint16_t) color) << 8 ) )

static inline uint8_t vga_mkcolor(enum vga_color fg, enum vga_color bg)
{
    return vga_colorentry(fg, bg);
}

static inline uint16_t vga_mkentry(char c, uint8_t color)
{
    return vga_create_entry(c, color);
}

vga_t vga =
{
    .bufaddr = (uint16_t*) VGA_MEMORY,
    .width   = VGA_WIDTH,
    .height  = VGA_HEIGHT,
    .color   = vga_colorentry(COLOR_WHITE, COLOR_BLACK)
};

void vga_setcolor(enum vga_color fg, enum vga_color bg)
{
    vga.color = vga_mkcolor(fg, bg);
}

void vga_putcharcolor(char c, size_t x, size_t y, enum vga_color fg, enum vga_color bg)
{
    const size_t index = y * vga.width + x;
    const uint8_t color = vga_mkcolor(fg, bg);
    vga.bufaddr[index] = vga_mkentry(c, color);
}

void vga_putchar(char c, size_t x, size_t y)
{
    const size_t index = y * vga.width + x;
    vga.bufaddr[index] = vga_mkentry(c, vga.color);
}

void vga_fillline(size_t y, const uint16_t* with)
{
    uint16_t *start_addr = vga.bufaddr + vga.width * y;

    uint16_t vga_defaultentry = vga_mkentry(' ', vga.color);

    for (size_t i = 0; i < vga.width; ++i) {
        start_addr[i] = (with != NULL) ? with[i] : vga_defaultentry;
    }
}

void vga_clearscreen(void)
{
    uint16_t vga_entry = vga_mkentry(' ', vga.color);
    uint16_t *start_addr = vga.bufaddr, *end_addr = vga.bufaddr + vga.width * vga.height;

    while (start_addr != end_addr) {
        *start_addr = vga_entry;
        ++start_addr;
    }
}

void vga_scrollup(size_t n, const uint16_t* fillwith)
{
    assert(n > 0);

    memcpy(vga.bufaddr, vga.bufaddr + vga.width, (vga.width * (vga.height - 1)) * sizeof(uint16_t));

    vga_fillline(vga.height - 1, fillwith);
}

void vga_scrolldown(size_t n, const uint16_t* fillwith)
{
    assert(n > 0);

    memcpyrev(vga.bufaddr + vga.width, vga.bufaddr, (vga.width * (vga.height - 1)) * sizeof(uint16_t));

    vga_fillline(0, fillwith);
}

void vga_mvcursor(size_t x, size_t y)
{
    uint16_t cursor_linear_pos;

    cursor_linear_pos = y * vga.width + x;

    /* Big endian architecture so we have to switch bytes */
    outb(VGA_CRTMICRO_INDEX_REG, CURSOR_LOCATION_HIGH_BYTE);
    outb(VGA_CRTMICRO_DATA_REG, (uint8_t) (cursor_linear_pos >> 8));

    outb(VGA_CRTMICRO_INDEX_REG, CURSOR_LOCATION_LOW_BYTE);
    outb(VGA_CRTMICRO_DATA_REG, (uint8_t) cursor_linear_pos);
}
