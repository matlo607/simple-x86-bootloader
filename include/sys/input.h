#ifndef _INPUT_H
#define _INPUT_H

#include <stddef.h>

void input_initialize(void);
char input_get_char(void);
size_t input_read(char* buf, size_t size);

#endif
