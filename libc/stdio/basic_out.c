#include <stdio.h>

#include <sys/tty.h>

void putc(char c)
{
    tty_putchar(c);
}

void putsndelim(const char* str, size_t n, char c)
{
    if (str) {
        for (size_t i = 0; i < n && str[i] != c; ++i) {
            putc(str[i]);
        }
    }
}

void putsdelim(const char* str, char delim)
{
    if (str) {
        char c;
        while ((c = *(str++)) != delim) {
            putc(c);
        }
    }
}
