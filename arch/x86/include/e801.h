#ifndef _MEMORY
#define _MEMORY

#include <stdint.h>
#include <stdbool.h>

typedef struct upper_mem_prop_s {
    uint16_t _1MB_to_16MB; // number of contiguous KB between 1 and 16 MB, maximum 0x3C00 = 15 MB
    uint16_t _16MB_to_4GB; // number of contiguous 64 KB blocks between 16 MB and 4 GB
} upper_memory_prop_t;

int16_t lower_memory_properties(void);
bool upper_memory_properties(upper_memory_prop_t* properties);

#endif
