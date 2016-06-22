#include "ctype.h"

#include <stdint.h>
#include <assert.h>

#define CT_UP   0x01    /* upper case */
#define CT_LOW  0x02    /* lower case */
#define CT_DIG  0x04    /* digit */
#define CT_CTL  0x08    /* control */
#define CT_PUN  0x10    /* punctuation */
#define CT_WHT  0x20    /* white space (space/cr/lf/tab) */
#define CT_HEX  0x40    /* hex digit */
#define CT_SP   0x80    /* hard space (0x20) */


static uint8_t _ctype[128] =
{
    CT_CTL,          // NUL (Null)                       0   0x00
    CT_CTL,          // SOH (Start of heading)           1   0x01
    CT_CTL,          // STX (Start of text)              2   0x02
    CT_CTL,          // ETX (End of text)                3   0x03
    CT_CTL,          // EOT (End of transmission)        4   0x04
    CT_CTL,          // ENQ (Enquiry)                    5   0x05
    CT_CTL,          // ACK (Acknowledge)                6   0x06
    CT_CTL,          // BEL (Bell)                       7   0x07
    CT_CTL,          // BS  (Backspace)                  8   0x08
    CT_CTL | CT_WHT, // TAB (Horizontal tabulation)      9   0x09
    CT_CTL | CT_WHT, // LF  (Line Feed, new line)        10  0x0A
    CT_CTL,          // VT  (Vertical tabulation)        11  0x0B
    CT_CTL,          // FF  (Form feed, new page)        12  0x0C
    CT_CTL | CT_WHT, // CR  (Carriage return)            13  0x0D
    CT_CTL,          // SO  (Shift out)                  14  0x0E
    CT_CTL,          // SI  (Shift in)                   15  0x0F
    CT_CTL,          // DLE (Data link escape)           16  0x10
    CT_CTL,          // DC1 (Device control 1)           17  0x11
    CT_CTL,          // DC2 (Device control 2)           18  0x12
    CT_CTL,          // DC3 (Device control 3)           19  0x13
    CT_CTL,          // DC4 (Device control 4)           20  0x14
    CT_CTL,          // NAK (Negative acknowledgement)   21  0x15
    CT_CTL,          // SYN (Synchronous idle)           22  0x16
    CT_CTL,          // ETB (End of transmission block)  23  0x17
    CT_CTL,          // CAN (Cancel)                     24  0x18
    CT_CTL,          // EM  (End of medium)              25  0x19
    CT_CTL,          // SUB (Substitute)                 26  0x1A
    CT_CTL,          // ESC (Escape)                     27  0x1B
    CT_CTL,          // FS (File separator)              28  0x1C
    CT_CTL,          // GS (Group separator)             29  0x1D
    CT_CTL,          // RS (Record separator)            30  0x1E
    CT_CTL,          // US (Unit separator)              31  0x1F
    CT_WHT | CT_SP,  // SP (Space)                       32  0x20
    CT_PUN,          // !                                33  0x21
    CT_PUN,          // "                                34  0x22
    CT_PUN,          // #                                35  0x23
    CT_PUN,          // $                                36  0x24
    CT_PUN,          // %                                37  0x25
    CT_PUN,          // &                                38  0x26
    CT_PUN,          // '                                39  0x27
    CT_PUN,          // (                                40  0x28
    CT_PUN,          // )                                41  0x29
    CT_PUN,          // *                                42  0x2A
    CT_PUN,          // +                                43  0x2B
    CT_PUN,          // ,                                44  0x2C
    CT_PUN,          // -                                45  0x2D
    CT_PUN,          // .                                46  0x2E
    CT_PUN,          // /                                47  0x2F
    CT_DIG,          // 0                                48  0x30
    CT_DIG,          // 1                                49  0x31
    CT_DIG,          // 2                                50  0x32
    CT_DIG,          // 3                                51  0x33
    CT_DIG,          // 4                                52  0x34
    CT_DIG,          // 5                                53  0x35
    CT_DIG,          // 6                                54  0x36
    CT_DIG,          // 7                                55  0x37
    CT_DIG,          // 8                                56  0x38
    CT_DIG,          // 9                                57  0x39
    CT_PUN,          // :                                58  0x3A
    CT_PUN,          // ;                                59  0x3B
    CT_PUN,          // <                                60  0x3C
    CT_PUN,          // =                                61  0x3D
    CT_PUN,          // >                                62  0x3E
    CT_PUN,          // ?                                63  0x3F
    CT_PUN,          // @                                64  0x40
    CT_HEX | CT_UP,  // A                                65  0x41
    CT_HEX | CT_UP,  // B                                66  0x42
    CT_HEX | CT_UP,  // C                                67  0x43
    CT_HEX | CT_UP,  // D                                68  0x44
    CT_HEX | CT_UP,  // E                                69  0x45
    CT_HEX | CT_UP,  // F                                70  0x46
    CT_UP,           // G                                71  0x47
    CT_UP,           // H                                72  0x48
    CT_UP,           // I                                73  0x49
    CT_UP,           // J                                74  0x4A
    CT_UP,           // K                                75  0x4B
    CT_UP,           // L                                76  0x4C
    CT_UP,           // M                                77  0x4D
    CT_UP,           // N                                78  0x4E
    CT_UP,           // O                                79  0x4F
    CT_UP,           // P                                80  0x50
    CT_UP,           // Q                                81  0x51
    CT_UP,           // R                                82  0x52
    CT_UP,           // S                                83  0x53
    CT_UP,           // T                                84  0x54
    CT_UP,           // U                                85  0x55
    CT_UP,           // V                                86  0x56
    CT_UP,           // W                                87  0x57
    CT_UP,           // X                                88  0x58
    CT_UP,           // Y                                89  0x59
    CT_UP,           // Z                                90  0x5A
    CT_PUN,          // [                                91  0x5B
    CT_PUN,          // \                                92  0x5C
    CT_PUN,          // ]                                93  0x5D
    CT_PUN,          // ^                                94  0x5E
    CT_PUN,          // _                                95  0x5F
    CT_PUN,          // '                                96  0x60
    CT_HEX | CT_LOW, // a                                97  0x61
    CT_HEX | CT_LOW, // b                                98  0x62
    CT_HEX | CT_LOW, // c                                99  0x63
    CT_HEX | CT_LOW, // d                                100 0x64
    CT_HEX | CT_LOW, // e                                101 0x65
    CT_HEX | CT_LOW, // f                                102 0x66
    CT_LOW,          // g                                103 0x67
    CT_LOW,          // h                                104 0x68
    CT_LOW,          // i                                105 0x69
    CT_LOW,          // j                                106 0x6A
    CT_LOW,          // k                                107 0x6B
    CT_LOW,          // l                                108 0x6C
    CT_LOW,          // m                                109 0x6D
    CT_LOW,          // n                                110 0x6E
    CT_LOW,          // o                                111 0x6F
    CT_LOW,          // p                                112 0x70
    CT_LOW,          // q                                113 0x71
    CT_LOW,          // r                                114 0x72
    CT_LOW,          // s                                115 0x73
    CT_LOW,          // t                                116 0x74
    CT_LOW,          // u                                117 0x75
    CT_LOW,          // v                                118 0x76
    CT_LOW,          // w                                119 0x77
    CT_LOW,          // x                                120 0x78
    CT_LOW,          // y                                121 0x79
    CT_LOW,          // z                                122 0x7A
    CT_PUN,          // {                                123 0x7B
    CT_PUN,          // |                                124 0x7C
    CT_PUN,          // }                                125 0x7D
    CT_PUN,          // ~                                126 0x7E
    CT_CTL           // DEL (delete)                     127 0x7F
};

bool isalnum(char c)
{
    return _ctype[(unsigned char) c] & (CT_UP | CT_LOW | CT_DIG);
}

bool isalpha(char c)
{
    return _ctype[(unsigned char) c] & (CT_UP | CT_LOW);
}

bool iscntrl(char c)
{
    return _ctype[(unsigned char) c] & (CT_CTL);
}

bool isdigit(char c)
{
    return _ctype[(unsigned char) c] & (CT_DIG);
}

bool isgraph(char c)
{
    return _ctype[(unsigned char) c] & (CT_PUN | CT_UP | CT_LOW | CT_DIG);
}

bool islower(char c)
{
    return _ctype[(unsigned char) c] & (CT_LOW);
}

bool isprint(char c)
{
    return _ctype[(unsigned char) c] & (CT_PUN | CT_UP | CT_LOW | CT_DIG | CT_SP);
}

bool ispunct(char c)
{
    return _ctype[(unsigned char) c] & (CT_PUN);
}

bool isspace(char c)
{
    return _ctype[(unsigned char) c] & (CT_WHT);
}

bool isupper(char c)
{
    return _ctype[(unsigned char) c] & (CT_UP);
}

bool isxdigit(char c)
{
    return _ctype[(unsigned char) c] & (CT_DIG | CT_HEX);
}

bool isascii(int c)
{
    return (c <= 0x7F);
}

bool isdigitinbase(char c, int base)
{
    assert( 2 <= base && base <= 16 );

    if ( isdigit(c) ) {
        if (base > 10) {
            return true;
        } else {
            return (c - '0') < base;
        }

    } else if ( isxdigit(c) ) {
        return (tolower(c) - 'a' + 10) < base;

    } else {
        return false;
    }
}

char toascii(int c)
{
    return (c & 0x7F);
}

char tolower(char c)
{
    return isupper(c) ? c + 'a' - 'A' : c ;
}

char toupper(char c)
{
    return islower(c) ? c + 'A' - 'a' : c ;
}
