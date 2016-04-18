#ifndef _INT12_H
#define _INT12_H

#include <stdint.h>

/*
 * Get the size of lower memory (memory before the EBDA (Extended BIOS Data Area) but
 * including also the real mode IVT (Interrupt Vector Table) and the BDA (BIOS Data
 * Area).
 *
 * In the lower memory (< 1MB), the memory guaranteed free for use lies between
 * the BDA (BIOS Data Area) and the EBDA (Extended BIOS Data Area).
 * The amount of memory (in kB) returned by the BIOS interrupt 0x12 in %ax is the
 * amount of RAM (just considering the lower memory) determined by POST minus the
 * memory allocated for the EBDA and the area allocated for the video memory and
 * BIOS rom.
 * Thus we have to substract from this amount the IVT and the BDA (0x500 bytes)
 *
 * */
int16_t int12_detect_lower_mem(void);

#endif
