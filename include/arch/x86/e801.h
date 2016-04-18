#ifndef _MEMORY
#define _MEMORY

#include <arch/x86/checkCCversion.h>

#include <stdint.h>
#include <stdbool.h>

typedef struct e801_upper_mem_s {
    uint16_t _1MB_to_16MB; // number of contiguous KB between 1 and 16 MB, maximum 0x3C00 = 15 MB
    uint16_t _16MB_to_4GB; // number of contiguous 64 KB blocks between 16 MB and 4 GB
} e801_upper_mem_t;

/*
 * The region of RAM above 1 MiB is not standardized, well-defined, or contiguous.
 * Some sub-regions of it contains memory mapped hardware, that nothing but a device
 * driver should ever access.
 * Area from : 0x00100000 to 0x00EFFFFF (14MB) -- free for use (if it exists)
 *             0x00F00000 to 0x00FFFFFF (1MB)  -- Possible memory mapped hardware
 *             0x00F00000 to 0x00FFFFFF (1MB)  -- ISA Memory Hole 15-16MB
 *             0x01000000 to ????????   (?MB)  -- free for use (if it exists)
 * When we go beyond 16MB, the best way to detect the memory of a PC
 * --> INT 0x15, %ax = 0xE820 command
 * However on old PCs, only INT 0x15, %ax = 0xE801 command is available.
 * */
bool e801_detect_upper_mem(e801_upper_mem_t* mem_properties);

#endif
