#ifndef _SYSTEM
#define _SYSTEM

#include "common.h"

#define interrupts_enable() \
    __asm__ __volatile__ ( "sti" )

#define interrupts_disable() \
    __asm__ __volatile__ ( "cli" )

#define halt_cpu() \
    __asm__ __volatile__ ( "hlt" )

#define reboot() \
    __asm__ __volatile__ ( "int $0x19" )


#define INITIAL_VIDEO_MODE_EGA_VGA          0x0
#define INITIAL_VIDEO_MODE_40x25_CGA        0x1
#define INITIAL_VIDEO_MODE_80x25_CGA        0x2
#define INITIAL_VIDEO_MODE_MONOCHROME       0x3

typedef struct equipment_info_s {
    uint16_t unused_bit0 : 1;
    uint16_t math_coprocessor : 1;
    uint16_t ps2_mouse : 1;
    uint16_t unused_bit3 : 1;
    uint16_t initial_video_mode : 2;
    uint16_t floppy_drives : 2;
    uint16_t unused_bit8 : 1;
    uint16_t nb_serial_adapters : 3;
    uint16_t game_adapter : 1;
    uint16_t unused_bit13 : 1;
    uint16_t nb_parallel_adapters : 2;
} equipment_info_t;

extern equipment_info_t equipment_get_info(void);
extern void equipment_print_info(equipment_info_t equipement);

#ifdef DEBUG

/*
 * \brief Abort the execution if assertion is false and print a message.
 * \param[in] cond    : assertion
 * \param[in] err_msg : error message to display (optional)
 *
 * */
extern void assert(bool cond, const char* error_msg);

#endif

#endif
