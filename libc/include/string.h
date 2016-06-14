#ifndef _STRING_H
#define _STRING_H

#include "stdint.h"
#include "stddef.h"

/*
 * \brief Convert an unsigned integer into string.
 * \param[in]    nb  : number to convert
 * \param[inout] str : string buffer (terminated by '\0')
 *
 * */
extern void uinttostring(uint64_t nb, char* str, uint8_t base);

/*
 * \brief Convert a signed integer into string.
 * \param[in]    nb   : number to convert
 * \param[inout] str  : string buffer (terminated by '\0')
 * \param[out]   base : base in which to display the number
 *
 * */
extern void inttostring(int64_t nb, char* str, uint8_t base);

/*
 * \brief Compute the length of a string.
 * \param[in] str : string buffer (terminated by '\0')
 * \return number of bytes in the string excluding the terminating '\0'
 *
 * */
extern size_t strlen(const char* str);

/*
 * \brief Locate character in string.
 * \param[in] str : string buffer
 * \param[in] c   : character to search
 * \return pointer on the first occurence
 *
 * */
extern char* strchr(const char* str, char c);

/*
 * \brief Compare two strings.
 * \param[in] s1 : first string
 * \param[in] s2 : second string
 * \return an integer less than, equal to, or greater than zero if s1 is found,
 * respectively, to be less than, to match, or be greater than s2
 *
 * */
extern ssize_t strcmp(const char* s1, const char* s2);

/*
 * \brief Compare at most n bytes of strings s1 and s2.
 * \param[in] s1 : first string
 * \param[in] s2 : second string
 * \param[in] n  : number of bytes to compare
 * \return an integer less than, equal to, or greater than zero if s1 (or the
 * first n bytes thereof) is found, respectively, to be less than, to match,
 * or be greater than s2
 *
 * */
extern ssize_t strncmp(const char* s1, const char* s2, size_t n);

/*
 * \brief Copy a string into another (until '\0' is found in src).
 * \param[inout] dest : destination string buffer
 * \param[in]    src  : original string
 * \return pointer on the last written character the destination string
 *
 * */
extern char* strcpy(char* dest, const char* src);

/*
 * \brief Copy a string into another (at most n characters if '\0' is not reached before).
 * \param[inout] dest : destination string buffer
 * \param[in]    src  : original string
 * \param[in]    n    : maximum number of bytes to copy
 * \return pointer on the last written character the destination string
 *
 * */
extern char* strncpy(char *dest, const char *src, size_t n);

/*
 * \brief Copy a string into another (until '\0' or the delimiter is found in src).
 * \param[inout] dest  : destination string buffer
 * \param[in]    src   : original string
 * \param[in]    delim : delimiter
 * \return pointer on the last written character in the destination string
 *
 * */
extern char* strcpydelim(char *dest, const char* src, char delim);

/*
 * \brief Copy a string into another (at most n characters if '\0' is not reached before).
 * \param[inout] dest  : destination string buffer
 * \param[in]    src   : original string
 * \param[in]    n     : maximum number of bytes to copy
 * \param[in]    delim : delimiter
 * \return pointer on the last written character the destination string
 *
 * */
extern char* strncpydelim(char *dest, const char* src, size_t n, char delim);

/*
 * \brief Reverse the bytes in a string.
 * \param[inout] str : string buffer
 * \param[in]    n   : number of bytes to reverse
 *
 * */
extern void strnrev(char* str, size_t n);

/*
 * \brief Fill memory with a constant byte.
 * \param[inout] mem : buffer
 * \param[in]    c   : constant byte
 * \param[in]    n   : number of bytes to set
 *
 * */
extern void memset(void* mem, uint8_t c, size_t n);

/*
 * \brief Copy n bytes from memory area src to memory area dest.
 * \param[inout] dest : destination buffer
 * \param[in]    src  : source buffer
 * \param[in]    n    : number of bytes to copy
 *
 * */
extern void memcpy(void *dest, const void *src, size_t n);

/*
 * \brief Copy n bytes from memory area src to memory area dest but in the reverse order that memcpy() would perform it.
 * \param[inout] dest : destination buffer
 * \param[in]    src  : source buffer
 * \param[in]    n    : number of bytes to copy
 *
 * */
extern void memcpyrev(void* dest, const void *src, size_t n);

#endif
