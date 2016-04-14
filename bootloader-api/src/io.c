#include "io.h"

#include "arg.h"
#include "assert.h"
#include "time.h"
#include "string.h"


char getc(void)
{
    char user_input;

    __asm__ __volatile__("movb %[bios_service_read_buffer], %%ah;"
                         "int $0x16;"
                         : "=a" (user_input)
                         : [bios_service_read_buffer] "i" (0x00)
                         : "cc"
                         );

    return user_input;
}

size_t getdelim(char* buf, size_t len, char delim, bool verbose)
{
    assert(buf != NULL, "getdelim : null pointer");

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

void putc(char c)
{
    __asm__ __volatile__ ("movb %[bios_service_print_char_active_page], %%ah;"
                          "movb %[char_to_print], %%al;"
                          "movb %[color], %%bl;"
                          "int $0x10;"
                          :
                          : [bios_service_print_char_active_page] "i" (0x0e),
                          [char_to_print] "o" (c),
                          [color] "i" (0x07)
                          : "%eax", "%ebx"
                          );
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

void printf(const char* format, ...)
{
    va_list args_list;
    va_start(args_list, format);

    const char* mark;
    const char* str_start = format;
    char var_format;
    bool flag_showbase;
    bool flag_zerofill;
    int8_t width;

    char buf_nbtostring[20];

    // parse format string looking for '%' character
    while ((mark = strchr(str_start, '%')) != NULL) {

        flag_showbase = false;
        flag_zerofill = false;
        width = -1; // -1 means "just displays a number according to
                    // its most significant bit different of zero"

        var_format = *(++mark);

        // check flags
        if (var_format == '#') {
            flag_showbase = true;
            var_format = *(++mark);
        }

        if (var_format == '0') {
            flag_zerofill = true;
            var_format = *(++mark);

            // check width (maximum 10 numbers)
            width = 0;
            while (var_format <= '9' && var_format >= '0' && width < 10) {
                width += (var_format - 48);
                var_format = *(++mark);
            }

            if (width == 0) {
                width = -1;
                flag_zerofill = false;
            }
        }

        switch (var_format) {
            case 's':
                {
                    char* p_str = va_arg(args_list, char*);

                    putsdelim(str_start, '%');
                    str_start = ++mark;

                    if (p_str == NULL) {
                        p_str = "null";
                    }
                    puts(p_str);

                    break;
                }

            case 'd':
                {
                    int32_t n = va_arg(args_list, int32_t);
                    inttostring(n, buf_nbtostring, 10);

                    putsdelim(str_start, '%');
                    str_start = ++mark;

                    if (flag_zerofill) {
                        size_t nchars = strlen(buf_nbtostring);
                        size_t nbzeros = width - nchars;
                        for (size_t i=0; i<nbzeros; ++i) {
                            putc('0');
                        }
                    }

                    puts(buf_nbtostring);

                    break;
                }

            case 'u':
                {
                    uint32_t n = va_arg(args_list, uint32_t);
                    uinttostring(n, buf_nbtostring, 10);

                    putsdelim(str_start, '%');
                    str_start = ++mark;

                    if (flag_zerofill) {
                        size_t nchars = strlen(buf_nbtostring);
                        size_t nbzeros = width - nchars;
                        for (size_t i=0; i<nbzeros; ++i) {
                            putc('0');
                        }
                    }

                    puts(buf_nbtostring);

                    break;
                }

            case 'p':
            case 'x':
                {
                    uint32_t n = va_arg(args_list, uint32_t);
                    uinttostring(n, buf_nbtostring, 16);

                    putsdelim(str_start, '%');
                    str_start = ++mark;

                    if (flag_showbase) {
                        puts("0x");
                    }

                    if (flag_zerofill) {
                        size_t nchars = strlen(buf_nbtostring);
                        size_t nbzeros = width - nchars;
                        for (size_t i=0; i<nbzeros; ++i) {
                            putc('0');
                        }
                    }

                    puts(buf_nbtostring);

                    break;
                }

            case 'b':
                {
                    uint32_t n = va_arg(args_list, uint32_t);
                    uinttostring(n, buf_nbtostring, 2);

                    putsdelim(str_start, '%');
                    str_start = ++mark;

                    if (flag_showbase) {
                        puts("1'b");
                    }

                    if (flag_zerofill) {
                        size_t nchars = strlen(buf_nbtostring);
                        size_t nbzeros = width - nchars;
                        for (size_t i=0; i<nbzeros; ++i) {
                            putc('0');
                        }
                    }

                    puts(buf_nbtostring);

                    break;
                }

            case 'c':
                {
                    char c = va_arg(args_list, char);

                    putsdelim(str_start, '%');
                    str_start = ++mark;

                    putc(c);

                    break;
                }

            case '%':
                {
                    putsdelim(str_start, '%');
                    str_start = ++mark;

                    putc('%');

                    break;
                }

            default:
                {
                    printf("unkown format symbol %c, abort()", var_format);
                    wait_forever();
                    break;
                }
        }
    }

    puts(str_start);

    va_end(args_list);
}
