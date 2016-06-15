#include <sys/tty.h>

#include <arch/x86/bios.h>
#include <sys/vga.h>
#include <string.h>

typedef struct {
    size_t row;
    size_t column;
    size_t width;
    size_t height;
    uint8_t color_background;
    uint8_t color_foreground;
} tty_t;

static tty_t this =
{
    .row = 0,
    .column = 0,
    .width = 25,
    .height = 80,
    .color_background = COLOR_BLACK,
    .color_foreground = COLOR_WHITE
};

static void scrollifneeded(void);

void tty_initialize(void)
{
    this.color_background = COLOR_BLACK;
    this.color_foreground = COLOR_GREEN;

#ifdef BOOTLOADER_PROTECTED_MODE_ENABLED

#ifdef PM_DRIVERS
    vga_setcolor(this.color_foreground, this.color_background);

    this.width = vga.width;
    this.height = vga.height;
#endif

#else
    this.width = 80;
    this.height = 25;
#endif

    tty_clearscreen();
    tty_showcursor();
}

void tty_settextcolor(enum vga_color foregroundcolor, enum vga_color backgroundcolor)
{
    this.color_foreground = foregroundcolor;
    this.color_background = backgroundcolor;

#ifdef BOOTLOADER_PROTECTED_MODE_ENABLED

#ifdef PM_DRIVERS
    vga_setcolor(foregroundcolor, backgroundcolor);
#endif

#endif
}

void tty_putchar(char c)
{
#ifdef BOOTLOADER_PROTECTED_MODE_ENABLED

#ifdef PM_DRIVERS
    if (c == '\r') {
        this.column = 0;
        vga_mvcursor(0, this.row);

    } else if (c == '\n') {
        this.row++;
        scrollifneeded();

    } else if (c == '\t') {
        size_t shifting = ((this.column + 4) & ~(4 - 1)) - this.column;
        for (size_t i = 0; i < shifting; ++i) {
            tty_putchar(' ');
        }

    } else {
        vga_putchar(c, this.column, this.row);
        this.column++;
        scrollifneeded();
    }
#else
    regs16_t regs;

    regs.a._Rl = c;
    regs.b._Rl = 0x07;
    regs.a._Rh = 0x0e;

    int32(0x10, &regs);
#endif

#else
    x86_regs_t regs_in, regs_out;

    x86_regs_init(&regs_in);

    regs_in.A._Rl = c;
    regs_in.B._Rl = 0x07;
    regs_in.A._Rh = 0x0e;

    bioscall(0x10, &regs_in, &regs_out);
#endif
}

void tty_clearscreen(void)
{
    this.row = 0;
    this.column = 0;
    vga_clearscreen();
    vga_mvcursor(0, 0);
}

void tty_showcursor(void)
{
    vga_mvcursor(this.column, this.row);
}

void tty_hidecursor(void)
{
    vga_mvcursor(CURSOR_HIDE_X, CURSOR_HIDE_Y);
}

void scrollifneeded(void)
{
    if ( this.column == this.width ) {
        this.column = 0;
        this.row++;
    }

    if ( this.row == this.height ) {
        vga_scrollup(1, NULL);
        this.row--;
    }

    vga_mvcursor(this.column, this.row);
}
