#ifndef _TTY_H
#define _TTY_H

#include <stddef.h>

void tty_initialize(void);
void tty_put_char(char c);
size_t tty_write(const char* buf, size_t size);

#endif
