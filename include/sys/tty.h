#ifndef _TTY_H
#define _TTY_H

#include <stddef.h>
#include <stdint.h>

void tty_initialize(void);
void tty_put_char(char c);
void tty_put_entry_at(char c, uint8_t color, size_t x, size_t y);
void tty_set_color(uint8_t color);
void tty_clscr(void);

#endif
