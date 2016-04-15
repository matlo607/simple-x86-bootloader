#include "string.h"
#include "assert.h"

void uinttostring(uint32_t nb, char* str, uint8_t base)
{
    uint32_t quotient;
    uint32_t remaining;
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

void inttostring(int32_t nb, char* str, uint8_t base)
{
    if (nb < 0) {
        str[0] = '-';
        ++str;
        nb = -nb;
    }

    uinttostring(nb, str, base);
}

size_t strlen(const char* str)
{
    size_t count = 0;

    while (*str) {
        ++str;
        ++count;
    }

    return count;
}

char* strchr(const char* str, char c)
{
    assert(str != NULL, "cannot search an occurrence in a null string");

    while (*str != c && *str != '\0') {
        ++str;
    }

    if (*str == c) {
        return (char*)str;
    } else {
        return NULL;
    }
}


ssize_t strcmp(const char* s1, const char* s2)
{
    if (s1 == NULL && s2 == NULL) {
        return 0;
    } else if (s1 == NULL) {
        return -1;
    } else if (s2 == NULL) {
        return 1;
    }

    while (*s1 != '\0' && *s2 != '\0' && *s1 == *s2) {
        s1++;
        s2++;
    }

    if (*s1 == *s2) {
        return 0;
    } else if (*s1 < *s2) { // if *s1 == '\0', it works too
        return -1;
    } else /* (*s1 > *s2) */ {
        return 1;
    }
}

ssize_t strncmp(const char* s1, const char* s2, size_t n)
{
    if (s1 == NULL && s2 == NULL) {
        return 0;
    } else if (s1 == NULL) {
        return -1;
    } else if (s2 == NULL) {
        return 1;
    }

    size_t i = 0;
    for (; i < n && (*s1 != '\0' && *s2 != '\0' && *s1 == *s2); i++) {
        s1++;
        s2++;
    }

    if (*s1 == *s2) {
        return 0;
    } else if (*s1 < *s2) { // if *s1 == '\0', it works too
        return -1;
    } else /* (*s1 > *s2) */ {
        return 1;
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

char* strncpy(char *dest, const char *src, size_t n)
{
    size_t i = 0;
    for (; i < n && src[i] != '\0'; ++i) {
        dest[i] = src[i];
    }
    dest[i] = '\0';

    return dest;
}

void strnrev(char* str, size_t n)
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

void memset(void* mem, uint8_t c, size_t n)
{
    if (mem != NULL) {
        for (size_t i = 0; i < n; ++i) {
            ((uint8_t*) mem)[i] = c;
        }
    }
}

void memcpy(void *dest, const void *src, size_t n)
{
    if (src != NULL) {
        for (size_t i = 0; i < n; ++i) {
            ((uint8_t*) dest)[i] = ((uint8_t*) src)[i];
        }
    }
}
