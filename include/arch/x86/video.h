#ifndef _BIOS_VIDEO_H
#define _BIOS_VIDEO_H

#include <arch/x86/checkCCversion.h>

#include <stdint.h>

typedef struct cursor_info_s {
    uint8_t row;
    uint8_t column;
    uint8_t top_line;
    uint8_t bottom_line;
} cursor_info_t;

typedef uint8_t video_mode_t;
                                                     // mode      | colors                     | Resolution            | Technology
#define VIDEO_MODE_TEXT_BW_40x25_320x200        0x00 // Text mode,  black and white,             40x25, 320x200          (CGA,EGA,MCGA,VGA)
#define VIDEO_MODE_TEXT_16C_40x25_320x200       0x01 // Text mode,  16 colors,                   40x25, 320x200          (CGA,EGA,MCGA,VGA)
#define VIDEO_MODE_TEXT_16G_80x25_640x200       0x02 // Text mode,  16 shades of gray,           80x25, 640x200          (CGA,EGA,MCGA,VGA)
#define VIDEO_MODE_TEXT_16C_80x25_640x200       0x03 // Text mode,  16 colors,                   80x25, 640x200          (CGA,EGA,MCGA,VGA)
#define VIDEO_MODE_GRAPHICS_4C1_40x25_320x200   0x04 // Graphics,   4 colors,                    40x25, 320x200          (CGA,EGA,MCGA,VGA)
#define VIDEO_MODE_GRAPHICS_4C2_40x25_320x200   0x05 // Graphics,   4 colors,                    40x25, 320x200          (CGA,EGA,MCGA,VGA)
#define VIDEO_MODE_GRAPHICS_2C_80x25_640x200    0x06 // Graphics,   2 colors (black and white),  80x25, 640x200          (CGA,EGA,MCGA,VGA)
#define VIDEO_MODE_TEXT_MONO_80x25              0x07 // Text mode,  monochrome,                  80x25                   (MDA,HERC,EGA,VGA)
#define VIDEO_MODE_GRAPHICS_16C_160x200         0x08 // Graphics,   16 colors,                   40x25, 160x200          (PCjr)
#define VIDEO_MODE_GRAPHICS_16C_320x200         0x09 // Graphics,   16 colors,                   40x25, 320x200          (PCjr)
#define VIDEO_MODE_GRAPHICS_16C_640x200         0x0a // Graphics,   4 colors,                    40x25, 640x200          (PCjr)
#define VIDEO_MODE_RESERVED_0x0b                0x0b // reserved
#define VIDEO_MODE_RESERVED_0x0c                0x0c // reserved
#define VIDEO_MODE_GRAPHICS_16C_40x25_320x200   0x0d // Graphics,   16 colors,                   40x25, 320x200          (EGA,VGA)
#define VIDEO_MODE_GRAPHICS_16C_80x25_640x200   0x0e // Graphics,   16 colors,                   80x25, 640x200          (EGA,VGA)
#define VIDEO_MODE_GRAPHICS_MONO_80x25          0x0f // Graphics,   monochrome,                  80x25                   (EGA,VGA)
#define VIDEO_MODE_GRAPHICS_4_16C_80x25_640x350 0x10 // Graphics,   4/16 colors,                 80x25, 640x350          (EGA or VGA with 128K)
#define VIDEO_MODE_GRAPHICS_2C_640x480          0x11 // Graphics,   2 colors,                    640x480                 (MCGA,VGA)
#define VIDEO_MODE_GRAPHICS_16C_640x480         0x12 // Graphics,   16 colors,                   640x480                 (VGA)
#define VIDEO_MODE_GRAPHICS_256C_640x480        0x13 // Graphics,   256 colors,                  320x200                 (MCGA,VGA)

typedef struct video_state_s {
    video_mode_t mode;
    uint8_t      char_column_nb;
    uint8_t      page;
} video_state_t;

/*
 * \brief Set video mode, then clear the screen and position the cursor at
 * the upper left hand corner of the screen (0,0) and resets the color
 * palette to known values.
 * \param[in] mode   : video mode to set
 *
 * BIOS interrupt: 0x10
 * service:        0x00
 *
 * Internal registers :
 *  %al : video mode
 * */
extern void video_setmode(video_mode_t mode);

/*
 * \brief Get video state.
 * \param[in] state : structure that will be filled with the current video state
 *
 * BIOS interrupt: 0x10
 * service:        0x0f
 *
 * Internal registers :
 *  %al : current video mode
 *  %ah : number of character columns
 *  %bh : active page
 * */
extern void video_getstate(video_state_t* state);

/*
 * \brief Draw a pixel on screen at the specified coordinates (column, row)
 * \param[in] column
 * \param[in] row
 * \param[in] color
 *
 * BIOS interrupt: 0x10
 * service:        0x0c
 *
 * Internal registers :
 *  %al : color
 *  %cx : column
 *  %dx : row
 * */
extern void video_draw_pixel(uint16_t column, uint16_t row, uint8_t color);

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
extern void video_setcursorpos(uint8_t page, uint8_t row, uint8_t column);

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
extern void video_getcursorpos(uint8_t page, cursor_info_t* info);

#endif
