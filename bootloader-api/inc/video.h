#ifndef _BIOS_VIDEO
#define _BIOS_VIDEO

#include "common.h"

typedef struct cursor_info_s {
    u8 row;
    u8 column;
    u8 top_line;
    u8 bottom_line;
} cursor_info_t;

typedef u8 video_mode_t;
                                                     // mode      | colors                     | Resolution            | Technology
#define VIDEO_MODE_TEXT_BW_40x25_320x200        0x00 // Text mode,  black and white,             40x25, 320x200          (CGA,EGA,MCGA,VGA)
#define VIDEO_MODE_TEXT_16C_40x25_320x200       0x01 // Text mode,  16 colors,                   40x25, 320x200          (CGA,EGA,MCGA,VGA)
#define VIDEO_MODE_TEXT_16G_80x25_640x200       0x02 // Text mode,  16 shades of gray,           80x25, 640x200          (CGA,EGA,MCGA,VGA)
#define VIDEO_MODE_TEXT_16C_80x25_640x200       0x03 // Text mode,  16 colors,                   80x25, 640x200          (CGA,EGA,MCGA,VGA)
#define VIDEO_MODE_GRAPHICS_4C1_40x25_320x200   0x04 // Graphics,   4 colors,                    40x25, 320x200          (CGA,EGA,MCGA,VGA)
#define VIDEO_MODE_GRAPHICS_4C2_40x25_320x200   0x05 // Graphics,   4 colors,                    40x25, 320x200          (CGA,EGA,MCGA,VGA)
#define VIDEO_MODE_GRAPHICS_2C_80x25_640x200    0x06 // Graphics,   2 colors (black and white),  80x25, 640x200          (CGA,EGA,MCGA,VGA)
#define VIDEO_MODE_TEXT_MONO_80x25              0x07 // Text mode,  monochrome,                  80x25                   (MDA,HERC,EGA,VGA)

#define VIDEO_MODE_RESERVED_0x0b                0x0b // reserved
#define VIDEO_MODE_RESERVED_0x0c                0x0c // reserved

//#define VIDEO_MODE_GRAPHICS_16C_40x25_320x200   0x0d // Graphics, 16 colors, 40x25, 320x200
//#define VIDEO_MODE_GRAPHICS_16C_80x25_640x200   0x0e // Graphics, 16 colors, 80x25, 640x200
//#define VIDEO_MODE_GRAPHICS_MONO_80x25          0x0f // Graphics, monochrome, 80x25
//#define VIDEO_MODE_GRAPHICS_4_16C_80x25_640x350 0x10 // Graphics, 4/16 colors, 80x25, 640x350

/*
 * \brief Set video mode, then clear the screen and position the cursor at
 * the upper left hand corner of the screen (0,0) and resets the color
 * palette to known values.
 * \param[in] mode   : video mode to set
 * \return new set video mode
 *
 * BIOS interrupt: 0x10
 * service:        0x00
 *
 * Internal registers :
 *  %al : video mode
 * */
extern video_mode_t video_setmode(video_mode_t mode);

/*
 * \brief Set cursor position on specified page, row and column.
 * \param[in] page   : page number
 * \param[in] row    : row on page
 * \param[in] column : column on page
 *
 * BIOS interrupt: 0x10
 * service:        0x02
 *
 * Internal registers :
 *  %bh : page number
 *  %dh : row
 *  %dl : column
 * */
extern void video_setcursorpos(u8 page, u8 row, u8 column);

/*
 * \brief Get cursor position on specified page, row and column.
 * \param[in]  page   : page number
 * \param[out] info   : pointer on a structure of type cursor_info_t
 *
 * BIOS interrupt: 0x10
 * service:        0x03
 *
 * Internal registers :
 *  %bh      : page number
 *  %ch, %cl : cursor type currently set
 *  %dh, %dl : row, column of current cursor on requested page
 * */
extern void video_getcursorpos(u8 page, cursor_info_t* info);

#endif
