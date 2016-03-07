#ifndef _BIOS_STRING
#define _BIOS_STRING

#include "common.h"

/*
 * \brief Convert an unsigned integer into string.
 * \param[in]    nb  : number to convert (0 < nb < 65535)
 * \param[inout] str : string buffer (terminated by '\0')
 *
 * */
extern void u16tostring(uint16_t nb, char* str, uint8_t base);

/*
 * \brief Convert a signed integer into string.
 * \param[in]    nb   : number to convert (-32768 < nb < 32767)
 * \param[inout] str  : string buffer (terminated by '\0')
 * \param[out]   base : base in which to display the number
 *
 * */
extern void s16tostring(int16_t nb, char* str, uint8_t base);

/*
 * \brief Compute the length of a string.
 * \param[in] str : string buffer (terminated by '\0')
 * \return number of bytes in the string excluding the terminating '\0'
 *
 * */
extern uint16_t strlen(const char* str);

/*
 * \brief Locate character in string.
 * \param[in] str : string buffer
 * \param[in] c   : character to search
 * \return pointer on the first occurence
 *
 * */
extern const char* strchr(const char* str, char c);

/*
 * \brief Copy a string into another (until '\0' is found in src).
 * \param[inout] dest : destination string buffer
 * \param[in]    src  : original string
 * \return pointer on the destination string
 *
 * */
extern char* strcpy(char* dest, const char* src);

/*
 * \brief Copy a string into another (at most n characters if '\0' is not reached before).
 * \param[inout] dest : destination string buffer
 * \param[in]    src  : original string
 * \param[in]    n    : number of bytes to copy
 * \return pointer on the destination string
 *
 * */
extern char* strncpy(char *dest, const char *src, uint16_t n);

/*
 * \brief Reverse the bytes in a string.
 * \param[inout] str : string buffer
 * \param[in]    n   : number of bytes to reverse
 *
 * */
extern void strnrev(char* str, uint16_t n);

/*
 * \brief Fill memory with a constant byte.
 * \param[inout] mem : buffer
 * \param[in]    c   : constant byte
 * \param[in]    n   : number of bytes to set
 *
 * */
extern void memset(void* mem, uint8_t c, uint16_t n);

#endif
