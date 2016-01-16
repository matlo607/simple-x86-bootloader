#include "screen.h"

#include "video.h"
#include "string.h"

void printc(char c, u8 page, u16 count)
{
    cursor_info_t info_cursor;
    video_getcursorpos(0, &info_cursor);

    __asm__ __volatile__("movb %[bios_service_print_char], %%ah;"
                         "movb %[char_to_print], %%al;"
                         "movb %[page], %%bh;"
                         "movw %[count], %%cx;"
                         "int $0x10;"
                         :
                         : [bios_service_print_char] "i" (0x0a),
                           [char_to_print] "g" (c),
                           [page] "g" (page),
                           [count] "g" (count)
                         );

    video_setcursorpos(0, info_cursor.row, info_cursor.column + count);
}

void printstrc(const char* str, char c)
{
    while (*str != c) {
        __asm__ __volatile__ (
                "int $0x10"
                :
                : "a"(0x0e00 | *str), "b"(0x0007)
                );
        ++str;
    }
}

void printstrn(const char* str, uint16_t n)
{
    for (uint16_t i = 0; i < n && str[i] != '\0'; ++i) {
        __asm__ __volatile__ (
                "int $0x10"
                :
                : "a"(0x0e00 | str[i]), "b"(0x0007)
                );
    }
}

void printf(const char* format, ...)
{
    const char* mark;
    const char* str_start = format;
    char var_format;

    char buf_nbtostring[20];

    // pointer on the arguments
    void* p = &format + 1;

    // parse format string looking for '%' character
    while ((mark = strchr(str_start, '%')) != NULL) {
        var_format = *(++mark);

        switch (var_format) {
            case 's':
                {
                    char** p_str = (char**)p;

                    printstrc(str_start, '%');
                    str_start = ++mark;

                    if (*p_str) {
                        prints(*p_str);
                    } else {
                        prints("null");
                    }

                    p = p_str + 1;
                    break;
                }

            case 'd':
                {
                    int16_t* p_s16 = (int16_t*)p;
                    s16tostring(*p_s16, buf_nbtostring, 10);

                    printstrc(str_start, '%');
                    str_start = ++mark;

                    prints(buf_nbtostring);

                    p = ((void**)p) + 1;
                    break;
                }

            case 'u':
                {
                    uint16_t* p_u16 = (uint16_t*)p;
                    u16tostring(*p_u16, buf_nbtostring, 10);

                    printstrc(str_start, '%');
                    str_start = ++mark;

                    prints(buf_nbtostring);

                    p = ((void**)p) + 1;
                    break;
                }

            case 'x':
                {
                    uint16_t* p_u16 = (uint16_t*)p;
                    u16tostring(*p_u16, buf_nbtostring, 16);

                    printstrc(str_start, '%');
                    str_start = ++mark;

                    prints(buf_nbtostring);

                    p = ((void**)p) + 1;
                    break;
                }

            case 'b':
                {
                    uint16_t* p_u16 = (uint16_t*)p;
                    u16tostring(*p_u16, buf_nbtostring, 2);

                    printstrc(str_start, '%');
                    str_start = ++mark;

                    prints(buf_nbtostring);

                    p = ((void**)p) + 1;
                    break;
                }

            case 'c':
                {
                    char* p_char = (char*)p;

                    printstrc(str_start, '%');
                    str_start = ++mark;

                    printc(*p_char, 0, 1);

                    p = ((void**)p) + 1;
                    break;
                }

            default:
            case '%':
                {
                    str_start = ++mark;
                    break;
                }
        }
    }

    prints(str_start);
}

