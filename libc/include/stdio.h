#ifndef _STDIO_H
#define _STDIO_H

#include "stddef.h"
#include "stdint.h"
#include "stdbool.h"
#include <stdarg.h>

/*
 * \brief Get a character from keyboard.
 *
 * \return character read from keyboard
 *
 * BIOS interrupt: 0x16
 * service:        0x01 and 0x00
 *
 * Internal registers :
 *  %al : character read from keyboard buffer
 * */
extern char getc(void);

/*
 * \brief Read a string from keyboard.
 * \param[in] buf   : buffer to store the string (a null-pointer check is performed)
 * \param[in] len   : length of the buffer
 * \param[in] delim : delimiter to stop reading from keyboard buffer
 *
 * \return number of characters read from keyboard
 * */
extern size_t getdelim(char* buf, size_t len, char delim, bool verbose);

#define getline(buf, len) \
    getdelim(buf, len, '\r', true)

/*
 * \brief Print a character on the active page.
 * \param[in] c     : character
 *
 * BIOS interrupt: 0x10
 * service:        0x0e
 *
 * Internal registers :
 *  %al : character to be written
 *  %bl : color in graphic mode
 * */
extern void putc(char c);

/*
 * \brief Print a string onto the active page until the character specified in
 * argument is found.
 * \param[in] str : pointer on the string to print
 * \param[in] c   : stop character
 *
 * BIOS interrupt: 0x10
 * service:        0x0e
 *
 * Internal registers :
 *  %al : character to be written
 *  %bl : color in graphic mode
 *  %bh : page number
 * */
extern void putsndelim(const char* str, size_t n, char c);

/*
 * \brief Print a string terminated by a NULL character onto the active page.
 * \param[in] STR : pointer on the string to print
 *
 * */
#define putsn(str, n)   putsndelim(str, n, '\0')

extern void putsdelim(const char* str, char c);

#define puts(str)       putsdelim(str, '\0')


/*
 * \brief Print the n first characters of a string onto the active page unless
 * a NULL character is found.
 * \param[in] str : pointer on the string to print
 * \param[in] n   : character count
 *
 * BIOS interrupt: 0x10
 * service:        0x0e
 *
 * Internal registers :
 *  %al : character to be written
 *  %bl : color in graphic mode
 *  %bh : page number
 * */
//extern void printstrn(const char* str, size_t n);

/*
 * \brief Print a string according to the pattern specified in the format.
 * \param[in] format   : string containing format options
 * \param[in] args ... : data to print into the string according to the format options
 *
 * */
extern void printf(const char* format, ...);

/*
 * \brief Print a string into another string according to the pattern specified in the format.
 * \param[inout] str   : destination string
 * \param[in] format   : string containing format options
 * \param[in] args ... : data to print into the string according to the format options
 *
 * */
extern void sprintf(char* str, const char* format, ...);


extern void vprintf(const char* format, va_list ap);
extern void vsprintf(char* str, const char* format, va_list ap);

#ifdef DEBUG
/*
 * \brief Macro enabled only if DEBUG has been defined. The behavior is exactly the same
 * than printf().
 * */
#define debug_printf(fmt, ...) do { printf(fmt, ##__VA_ARGS__); } while (0)
#endif

typedef uint32_t FILE;

extern FILE* fopen(const char* path, const char* mode);
extern size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream);
extern size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream);
extern int fclose(FILE* fp);

#endif
