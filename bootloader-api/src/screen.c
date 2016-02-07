#include "screen.h"

#include "video.h"
#include "string.h"

void printc(char c, uint16_t count)
{
    for (uint16_t i = 0; i < count; ++i) {
        __asm__ __volatile__ ("movb %[bios_service_print_char_active_page], %%ah;"
                              "movb %[char_to_print], %%al;"
                              "movb %[color], %%bl;"
                              "int $0x10;"
                              :
                              : [bios_service_print_char_active_page] "i" (0x0e),
                                [char_to_print] "g" (c),
                                [color] "i" (0x07)
                              : "%eax", "%ebx"
                             );
    }
}

void printstrc(const char* str, char c)
{
    while (*str != c) {
        __asm__ __volatile__ (
                "movb %[bios_service_print_char_active_page], %%ah;"
                "movb %[char_to_print], %%al;"
                "movb %[color], %%bl;"
                "int $0x10;"
                :
                : [bios_service_print_char_active_page] "i" (0x0e),
                  [char_to_print] "g" (*str),
                  [color] "i" (0x07)
                : "%eax", "%ebx"
                );
        ++str;
    }
}

void printstrn(const char* str, uint16_t n)
{
    for (uint16_t i = 0; i < n && str[i] != '\0'; ++i) {
        __asm__ __volatile__ (
                "movb %[bios_service_print_char_active_page], %%ah;"
                "movb %[char_to_print], %%al;"
                "movb %[color], %%bl;"
                "int $0x10;"
                :
                : [bios_service_print_char_active_page] "i" (0x0e),
                  [char_to_print] "m" (str[i]),
                  [color] "i" (0x07)
                : "%eax", "%ebx"
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

                    printc(*p_char, 1);

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

