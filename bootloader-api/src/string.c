#include "string.h"

void u16tostring(uint16_t nb, char* str, uint8_t base)
{
    uint16_t quotient;
    uint16_t remaining;
    uint8_t nbdigits = 0;

    do {
        quotient = nb / base;
        remaining = nb % base;

        remaining += '0';
        if (remaining > '9') {
            remaining += 7;
        }
        str[nbdigits] = remaining;
        ++nbdigits;

        nb = quotient;
    } while (quotient != 0);

    strnrev(str, nbdigits);
    str[nbdigits] = '\0';
}

void s16tostring(int16_t nb, char* str, uint8_t base)
{
    if (nb < 0) {
        str[0] = '-';
        ++str;
        nb = -nb;
    }

    u16tostring(nb, str, base);
}

uint16_t strlen(const char* str)
{
    uint16_t count = 0;

    while (*str) {
        ++str;
        ++count;
    }

    return count;
}

const char* strchr(const char* str, char c)
{
    while (*str != c && *str != '\0') {
        ++str;
    }

    if (*str == c) {
        return str;
    } else {
        return NULL;
    }
}

char* strcpy(char* dest, const char* src)
{
    char c;

    do {
        c = *src;
        *dest = c;
        ++dest;
        ++src;
    } while (c != '\0');

    return dest;
}

char* strncpy(char *dest, const char *src, uint16_t n)
{
    uint16_t i = 0;
    for (; i < n && src[i] != '\0'; ++i) {
        dest[i] = src[i];
    }
    dest[i] = '\0';

    return dest;
}

void strnrev(char* str, uint16_t n)
{
    char* str_end = &str[n-1];
    char tmp;

    while (str != str_end && str < str_end) {
        tmp = *str;
        *str = *str_end;
        *str_end = tmp;

        ++str;
        --str_end;
    }
}

