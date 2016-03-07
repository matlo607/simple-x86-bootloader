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


/*
 * Dynamic allocation's functions
 **/

#define HEAP_MAX_SIZE   4096 // 4kB

extern void* malloc(uint16_t size);
extern void* calloc(uint16_t nmemb, uint16_t size);
extern void* realloc(void *ptr, uint16_t size);
extern void free(void *ptr);



typedef struct SMAP_entry_s {
    uint32_t baseL; // base address uint64_t
    uint32_t baseH;
    uint32_t lengthL; // length uint64_t
    uint32_t lengthH;
    uint32_t type; // entry Type
    uint32_t ACPI; // extended
} __attribute__((packed)) SMAP_entry_t;

int16_t memory_map_int15_E820(SMAP_entry_t* buffer, int16_t nb_max_entries);

typedef struct upper_mem_prop_s {
    uint16_t _1MB_to_16MB; // number of contiguous KB between 1 and 16 MB, maximum 0x3C00 = 15 MB
    uint16_t _16MB_to_4GB; // number of contiguous 64 KB blocks between 16 MB and 4 GB
} upper_memory_prop_t;

extern int16_t lower_memory_properties(void);
extern bool upper_memory_properties(upper_memory_prop_t* properties);

#endif
