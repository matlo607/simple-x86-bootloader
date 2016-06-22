#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/system.h>


void sprintf(char* str, const char* format, ...)
{
    va_list args_list;
    va_start(args_list, format);

    vsprintf(str, format, args_list);

    va_end(args_list);
}

void vsprintf(char* str, const char* format, va_list ap)
{
    const char* mark;
    const char* str_start = format;
    char var_format;
    bool long_nb;
    bool flag_showbase;
    bool flag_zerofill;
    int8_t width;

    char buf_nbtostring[65];

    // parse format string looking for '%' character
    while ((mark = strchr(str_start, '%')) != NULL) {

        long_nb = false;
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

            // check width (maximum 20 numbers)
            width = 0;
            while (var_format <= '9' && var_format >= '0' && width < 21) {
                width += (var_format - 48);
                var_format = *(++mark);
            }

            if (width == 0) {
                width = -1;
                flag_zerofill = false;
            }
        }

        if (var_format == 'l') {
            long_nb = true;
            var_format = *(++mark);
        }

        char c;
        char* p_str = NULL;
        int64_t signed_n;
        uint64_t unsigned_n;

        // get the value according to the specified format
        switch (var_format) {
            case 'c':
                {
                    c = va_arg(ap, uint32_t);
                    break;
                }

            case 's':
                {
                    p_str = va_arg(ap, char*);
                    break;
                }

            case 'd':
                {
                    if (long_nb) {
                        signed_n = va_arg(ap, int64_t);
                    } else {
                        signed_n = va_arg(ap, int32_t);
                    }
                    inttostring(signed_n, buf_nbtostring, 10);

                    break;
                }

            case 'b':
            case 'p':
            case 'u':
            case 'x':
                {
                    if (long_nb) {
                        unsigned_n = va_arg(ap, uint64_t);
                    } else {
                        unsigned_n = va_arg(ap, uint32_t);
                    }

                    if (var_format == 'p' || var_format == 'x') {
                        uinttostring(unsigned_n, buf_nbtostring, 16);
                    } else {
                        uinttostring(unsigned_n, buf_nbtostring, 10);
                    }

                    break;
                }

            case '%':
                {
                    // nothing to do here, the value is '%'
                    break;
                }

            default:
                {
                    panic("unkown format symbol %%%c", var_format);
                    break;
                }
        }

        // print the string until the delimiter
        str = strcpydelim(str, str_start, '%');
        str_start = ++mark;

        // print base (only for 'p','x' and 'b')
        if (flag_showbase) {
            if (var_format == 'p' || var_format == 'x') {
                str = strcpy(str, "0x");
            } else if (var_format == 'b') {
                str = strcpy(str, "1'b");
            }
        }

        // print the value
        switch (var_format) {
            case 'c':
                {
                    *(str++) = c;
                    *str = '\0';

                    break;
                }

            case 's':
                {
                    if (p_str == NULL) {
                        p_str = "null";
                    }
                    str = strcpy(str, p_str);

                    break;
                }

            case 'b':
            case 'd':
            case 'u':
            case 'p':
            case 'x':
                {
                    // padding with 0
                    if (flag_zerofill) {
                        size_t nchars = strlen(buf_nbtostring);
                        size_t nbzeros = width - nchars;
                        for (size_t i=0; i<nbzeros; ++i) {
                            *(str++) = '0';
                        }
                        *str = '\0';
                    }

                    str = strcpy(str, buf_nbtostring);

                    break;
                }

            case '%':
                {
                    *(str++) = '%';
                    *str = '\0';

                    break;
                }
        }
    }

    str = strcpy(str, str_start);
}
