#ifndef _SYS_VGA_H
#define _SYS_VGA_H

#ifdef PM_DRIVERS

#include <stdint.h>
#include <stddef.h>
#include <sys/colors.h>

#define CURSOR_HIDE_X -1
#define CURSOR_HIDE_Y -1

typedef struct {
    uint16_t* const bufaddr;
    const size_t width;
    const size_t height;
    uint8_t color;
} vga_t;

extern vga_t vga;

extern void vga_setcolor(enum vga_color fg, enum vga_color bg);

extern void vga_putcharcolor(char c, size_t x, size_t y, enum vga_color fg, enum vga_color bg);
extern void vga_putchar(char c, size_t x, size_t y);

extern void vga_fillline(size_t y, const uint16_t* with);
extern void vga_clearscreen(void);

extern void vga_scrollup(size_t n, const uint16_t* fillwith);
extern void vga_scrolldown(size_t n, const uint16_t* fillwith);

extern void vga_mvcursor(size_t x, size_t y);

#endif

#endif
