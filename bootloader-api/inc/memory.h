#ifndef _MEMORY
#define _MEMORY

#include "common.h"

#define mem_set_data_segment(ADDR) \
    __asm__ __volatile__ ( "movw %w0, %%ds" : : "r" (ADDR) )

#define mem_set_extra_data_segment(ADDR) \
    __asm__ __volatile__ ( "movw %w0, %%es" : : "r" (ADDR) )

#define mem_set_stack_segment(ADDR) \
    __asm__ __volatile__ ( "movw %w0, %%ss" : : "r" (ADDR) )

#define mem_set_stack_pointer(ADDR) \
    __asm__ __volatile__ ( "movw %w0, %%sp" : : "r" (ADDR) )

#define mem_set_stack_base_pointer(ADDR) \
    __asm__ __volatile__ ( "movw %w0, %%bp" : : "r" (ADDR) )


typedef struct upper_mem_prop_s {
    uint16_t _1MB_to_16MB; // number of contiguous KB between 1 and 16 MB, maximum 0x3C00 = 15 MB
    uint16_t _16MB_to_4GB; // number of contiguous 64 KB blocks between 16 MB and 4 GB
} upper_memory_prop_t;

extern int16_t lower_memory_properties(void);
extern bool upper_memory_properties(upper_memory_prop_t* properties);

#endif
