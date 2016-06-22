#ifndef _CTYPE_H
#define _CTYPE_H

#include <stdbool.h>

extern bool isalnum(char c);
extern bool isalpha(char c);
extern bool iscntrl(char c);
extern bool isdigit(char c);
extern bool isgraph(char c);
extern bool islower(char c);
extern bool isprint(char c);
extern bool ispunct(char c);
extern bool isspace(char c);
extern bool isupper(char c);
extern bool isxdigit(char c);
extern bool isascii(int c);
extern bool isdigitinbase(char c, int base);
extern char toascii(int c);
extern char tolower(char c);
extern char toupper(char c);

#endif
