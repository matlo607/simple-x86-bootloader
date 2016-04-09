#ifndef _E820_H
#define _E820_H

#include "common.h"

typedef struct SMAP_entry_s {
    uint32_t baseL; // base address uint64_t
    uint32_t baseH;
    uint32_t lengthL; // length uint64_t
    uint32_t lengthH;
    uint32_t type; // entry Type
    uint32_t ACPI; // extended
} __attribute__((packed)) SMAP_entry_t;

ssize_t e820_detect_mem(SMAP_entry_t* buffer, ssize_t nb_max_entries);
void e820_display_SMAP_entries(SMAP_entry_t* buffer, size_t n);

#endif
