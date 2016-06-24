#ifndef _PIC_H
#define _PIC_H

#include <stdint.h>

#define MASK_DISABLE_ALL_INTERRUPTS 0xffff
#define MASK_ENABLE_ALL_INTERRUPTS  0x0

#define mask_enableinterrupt(current_mask, intno) \
    ((current_mask) & ~(1 << (intno)))

#define mask_disableinterrupt(current_mask, intno) \
    ((current_mask) | (1 << (intno)))

extern void i86_pic_remap(uint8_t pic1_vecoffset, uint8_t pic2_vecoffset, uint16_t mask);
extern uint16_t i86_pic_getintmask(void);
extern void i86_pic_setintmask(uint16_t mask);
extern void i86_pic_endofinterrupt(uint8_t intno);

#endif
