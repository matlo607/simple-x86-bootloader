#include "string.h"
#include "assert.h"

size_t strlen(const char* str)
{
    assert(str != NULL);

    size_t count = 0;

    while (*str) {
        ++str;
        ++count;
    }

    return count;
}

char* strchr(const char* str, char c)
{
    assert(str != NULL);

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
    assert(s1 != NULL);
    assert(s2 != NULL);

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
    assert(s1 != NULL);
    assert(s2 != NULL);

    size_t i = 0;
    while ( (i < n) && (*s1 != '\0' && *s2 != '\0' && *s1 == *s2) ) {
        if (++i < n) {
            s1++;
            s2++;
        }
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
    assert(dest != NULL);
    assert(src != NULL);

    char c;

    while ( (c = *src) != '\0' ) {
        *dest = c;
        ++dest;
        ++src;
    }

    return dest;
}

char* strncpy(char *dest, const char *src, size_t n)
{
    assert(dest != NULL);
    assert(src != NULL);

    size_t i = 0;
    for (; i < n && src[i] != '\0'; ++i) {
        dest[i] = src[i];
    }

    dest[i] = '\0';

    return &dest[i];
}

char* strcpydelim(char *dest, const char* src, char delim)
{
    assert(dest != NULL);
    assert(src != NULL);

    char c;
    while ( ( (c = *src) != delim ) && ( c != '\0') ) {
        *dest = c;
        ++dest;
        ++src;
    }

    *dest = '\0';

    return dest;
}

char* strncpydelim(char *dest, const char* src, size_t n, char delim)
{
    assert(dest != NULL);
    assert(src != NULL);

    size_t i = 0;
    for (; ( i < n )
            && ( src[i] != delim )
            && (src[i] != '\0');
            ++i) {
        dest[i] = src[i];
    }

    dest[i] = '\0';

    return &dest[i];
}

void strnrev(char* str, size_t n)
{
    assert(str != NULL);

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
