#include <string.h>
#include <assert.h>
#include <ctype.h>

uint64_t strtoull(const char *str, char **endptr, uint8_t base)
{
    assert(str != NULL);
    assert( 2 <= base && base <= 16 );

    uint64_t n = 0;

    char c = *str;
    while ( isdigitinbase(c, base) ) {

        n *= base;

        if ( isdigit(c) ) {
            n += c - '0';
        } else {
            c = tolower(c);
            n += c - 'a' + 10;
        }

        ++str;
        c = *str;
    }

    if (endptr != NULL) {
        *endptr = (char*) str;
    }

    return n;
}

int64_t strtoll(const char *str, char **endptr, uint8_t base)
{
    assert(str != NULL);
    assert( 2 <= base && base <= 16 );

    int64_t n = 0;
    bool negative = false;

    if (str[0] == '-') {
        ++str;
        negative = true;
    }

    n = strtoull(str, endptr, base);

    return (negative) ? -n : n;
}
