#ifndef _BIOS_INPUT_OUTPUT
#define _BIOS_INPUT_OUTPUT

#include "common.h"

/*
 * \brief Print a character on the active page a certain number of times.
 * \param[in] c     : character
 * \param[in] count : count of characters to be printed
 *
 * BIOS interrupt: 0x10
 * service:        0x0e
 *
 * Internal registers :
 *  %al : character to be written
 *  %bl : color in graphic mode
 * */
extern void printc(char c, uint16_t count);

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
extern void printstrc(const char* str, char c);

/*
 * \brief Print a string terminated by a NULL character onto the active page.
 * \param[in] STR : pointer on the string to print
 *
 * */
#define prints(STR)     printstrc(STR, '\0')

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
extern void printstrn(const char* str, uint16_t n);

/*
 * \brief Print a string according to the pattern specified in the format.
 * \param[in] format   : string containing format options
 * \param[in] args ... : data to print into the string according to the format options
 *
 * */
extern void printf(const char* format, ...);

#endif
