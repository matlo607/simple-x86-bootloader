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

        // increment the number of read characters if it is not a backspace
        if (c != '\b') {
            buf[n] = c;
            ++n;
        }

        // if this is a delimiter, stop reading
        if (c == delim) {
            putc('\r');
            putc('\n');
            break;
        }

        // display the character (backspace included)
        if (verbose && n > 0) {
            putc(c);
        }

        // backspace: remove the previous char from buffer
        if (c == '\b' && n > 0) {
            --n;
        }
    }

    if (n < len) {
        buf[n] = '\0';
    }

    return n;
}
