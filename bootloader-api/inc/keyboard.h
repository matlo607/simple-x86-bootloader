#ifndef _BIOS_KEYBOARD
#define _BIOS_KEYBOARD

#include "common.h"

/*
 * \brief Wait a keystroke on keyboard.
 *
 * \note Remove the stroke from buffer.
 * */
#if 0 // replaced by a define using keyboard_getc()
extern void keyboard_waitkeystroke(void);
#endif
#define keyboard_waitkeystroke() \
    keyboard_getc(false)

/*
 * \brief Get a character from keyboard.
 * \param[in] display_stroke : boolean to enable/disable the displaying of the stroke
 * \return character read from keyboard
 *
 * BIOS interrupt: 0x16
 * service:        0x01 and 0x00
 *
 * Internal registers :
 *  %al : character read from keyboard buffer
 * */
extern char keyboard_getc(bool display_stroke);

/*
 * \brief Read a string from keyboard.
 * \param[in] buf   : buffer to store the string (a null-pointer check is performed)
 * \param[in] len   : length of the buffer
 * \param[in] delim : delimiter to stop reading from keyboard buffer
 * \return number of characters read from keyboard
 *
 * \note If the specified delimiter is a '\n', then it is replaced by a '\0'
 * */
extern uint16_t keyboard_getline(char* buf, uint16_t len, uint8_t delim);

#endif
