#ifndef _TTY_H
#define _TTY_H

#include <stddef.h>
#include <stdint.h>
#include <sys/colors.h>

extern void tty_initialize(void);
extern void tty_putchar(char c);
extern void tty_settextcolor(enum vga_color foregroundcolor, enum vga_color backgroundcolor);
extern void tty_clearscreen(void);
extern void tty_showcursor(void);
extern void tty_hidecursor(void);

#endif
