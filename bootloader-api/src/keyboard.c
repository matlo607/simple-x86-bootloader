#include "keyboard.h"
#include "screen.h"
#include "system.h"

//#define _QWERTY_to_AZERTY

#ifdef _QWERTY_to_AZERTY
static char key_mapping_QWERTYtoAZERTY[94] =
{
    '1' /* '!' */,
    '%' /* '"' */,
    '3' /* '#' */,
    '4' /* '$' */,
    '5' /* '%' */,
    '7' /* '&' */,
    '<' /* '\'' */,
    '9' /* '(' */,
    '0' /* ')' */,
    '8' /* '*' */,
    '+' /* '+' */,
    ';' /* ',' */,
    ')' /* '-' */,
    'm' /* '.' */,
    '!' /* '/' */,
    '@' /* '0' */,
    '&' /* '1' */,
    '~' /* '2' */,
    '"' /* '3' */,
    '\'' /* '4' */,
    '(' /* '5' */,
    '-' /* '6' */,
    '`' /* '7' */,
    '_' /* '8' */,
    '^' /* '9' */,
    '?' /* ':' */,
    '.' /* ';' */,
    ';' /* '<' */,
    '=' /* '=' */,
    '>' /* '>' */,
    '?' /* '?' */,
    '2' /* '@' */,
    'Q' /* 'A' */,
    'B' /* 'B' */,
    'C' /* 'C' */,
    'D' /* 'D' */,
    'E' /* 'E' */,
    'F' /* 'F' */,
    'G' /* 'G' */,
    'H' /* 'H' */,
    'I' /* 'I' */,
    'J' /* 'J' */,
    'K' /* 'K' */,
    'L' /* 'L' */,
    ':' /* 'M' */,
    'N' /* 'N' */,
    'O' /* 'O' */,
    'P' /* 'P' */,
    'A' /* 'Q' */,
    'R' /* 'R' */,
    'S' /* 'S' */,
    'T' /* 'T' */,
    'U' /* 'U' */,
    'V' /* 'V' */,
    'Z' /* 'W' */,
    'X' /* 'X' */,
    'Y' /* 'Y' */,
    'W' /* 'Z' */,
    '^' /* '[' */,
    '*' /* '\\' */,
    '$' /* ']' */,
    '6' /* '^' */,
    '_' /* '_' */,
    '`' /* '`' */,
    'q' /* 'a' */,
    'b' /* 'b' */,
    'c' /* 'c' */,
    'd' /* 'd' */,
    'e' /* 'e' */,
    'f' /* 'f' */,
    'g' /* 'g' */,
    'h' /* 'h' */,
    'i' /* 'i' */,
    'j' /* 'j' */,
    'k' /* 'k' */,
    'l' /* 'l' */,
    ',' /* 'm' */,
    'n' /* 'n' */,
    'o' /* 'o' */,
    'p' /* 'p' */,
    'a' /* 'q' */,
    'r' /* 'r' */,
    's' /* 's' */,
    't' /* 't' */,
    'u' /* 'u' */,
    'v' /* 'v' */,
    'z' /* 'w' */,
    'x' /* 'x' */,
    'y' /* 'y' */,
    'w' /* 'z' */,
    '{' /* '{' */,
    '>' /* '|' */,
    '}' /* '}' */,
    '~' /* '~ */
};
#endif

/* FIX ME : this function does not work, waiting time is ignored */
void keyboard_waitkeystroke(void)
{
    __asm__ __volatile__("0: movb %[bios_service_wait_key_stroke], %%ah;"
                         "int $0x16;"
                         "jc 0b;"
                         :
                         : [bios_service_wait_key_stroke] "i" (0x01)
                         : "cc"
                         );
}

char keyboard_getc(bool display_stroke)
{
    char user_input;

    __asm__ __volatile__("0: movb %[bios_service_wait_key_stroke], %%ah;"
                         "int $0x16;"
                         "jc 0b;"
                         "movb %[bios_service_read_buffer], %%ah;"
                         "int $0x16;"
                         : "=a" (user_input)
                         : [bios_service_wait_key_stroke] "i" (0x01),
                           [bios_service_read_buffer] "i" (0x00)
                         : "cc"
                         );

#ifdef _QWERTY_to_AZERTY
    /* if the keyboard mapping is not QWERTY but AZERTY */
    user_input = key_mapping_QWERTYtoAZERTY[user_input-'!'];
#endif


    if (display_stroke) {
        printc(user_input, 0, 1);
    }

    return user_input;
}

uint16_t keyboard_getline(char* buf, uint16_t len, uint8_t delim)
{
    assert(buf != NULL, "keyboard_getline : null pointer");

    uint16_t i = 0;

    while (i < len) {
        char c = keyboard_getc(false);
        if (c != delim) {
            printc(c, 0, 1);
            buf[i] = c;
            ++i;
        } else if (c == '\r') {
            buf[i] = '\0';
            ++i;
            break;
        }
        else {
            break;
        }

    }

    return i;
}

