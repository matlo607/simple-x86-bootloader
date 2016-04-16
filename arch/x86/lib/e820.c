#include "e820.h"

#include "cpu.h"
#include <stdio.h>


enum MemRegion_t {
    MEM_USABLE           = 1,
    MEM_RESERVED         = 2,
    MEM_ACPI_RECLAIMABLE = 3,
    MEM_ACPI_NVS         = 4
};


static const char* memregtostr(enum MemRegion_t type)
{
    switch(type)
    {
        case MEM_USABLE:
             return "usable";
        case MEM_RESERVED:
             return "reserved";
        case MEM_ACPI_RECLAIMABLE:
             return "ACPI reclaimable";
        case MEM_ACPI_NVS:
             return "ACPI NVS";
        default:
             return "unknown";
    }
}

void e820_display_SMAP_entries(SMAP_entry_t* buffer, size_t n)
{
    for (size_t i = 0; i < n; ++i) {
        printf("[%u] type=%s\r\n" \
               "\tbase address: 0x%x%x\r\n" \
               "\tlength : %u\r\n",
               i,
               memregtostr(buffer->type),
               buffer->baseH, buffer->baseL,
               buffer->lengthL);
        ++buffer;
    }
}

ssize_t e820_detect_mem(SMAP_entry_t* buffer, ssize_t nb_max_entries)
{
    uint32_t continuation_val = 0;
    uint32_t signature;
    uint32_t bytes = 0;

    ssize_t nb_entries = 0;

    uint16_t reg_ds = 0;
    reg_get_data_segment(&reg_ds);

    reg_set_extended_data_segment(reg_ds);

    do {

        __asm__ __volatile__ (
                              "xor %%eax, %%eax;"
                              "movw %[bios_service_e820], %%ax;"
                              "movl %[continuation_val_in], %%ebx;"
                              "movl %[struct_size], %%ecx;"
                              "movl %[signature_in], %%edx;"
                              "movw %[buffer], %%di;"
                              "int $0x15;"
                              "jc 1f;"
                              "movl %%eax, %[signature_out];"
                              "movl %%ebx, %[continuation_val_out];"
                              "movl %%ecx, %[bytes];"
                              "1:"
                              : [signature_out] "=o" (signature),
                                [continuation_val_out] "=o" (continuation_val),
                                [bytes] "=o" (bytes)
                              : [bios_service_e820] "i" (0xe820),
                                [continuation_val_in] "o" (continuation_val),
                                [struct_size] "i" (sizeof(SMAP_entry_t)),
                                [signature_in] "i" (0x534D4150),
                                [buffer] "o" (buffer)
                              : "cc");

        if (signature != 0x534D4150) {
            nb_entries = -1;
            break;

        } else if (bytes > 20 && (buffer->ACPI & 0x1) == 0x0) {
            // Ignore the entry

            // Explanation :
            // bit 0 of the Extended Attributes indicates if the entire entry should be ignored (if the bit is clear)

        } else {
            nb_entries++;
            buffer++;
        }

    } while (continuation_val != 0 && nb_entries < nb_max_entries);

    return nb_entries;
}

