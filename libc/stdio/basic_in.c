#include <stdio.h>
#include <assert.h>

#include <sys/input.h>

char getc(void)
{
    return input_get_char();
}

size_t getdelim(char* buf, size_t len, char delim, bool verbose)
{
    assert(buf != NULL);

    size_t n = 0;

    while (n < len) {
        char c = getc();

        buf[n] = c;
        ++n;

        if (c == delim) {
            putc('\r');
            putc('\n');
            break;
        }

        if (verbose) {
            putc(c);
        }
    }

    if (n < len) {
        buf[n] = '\0';
    }

    return n;
}
