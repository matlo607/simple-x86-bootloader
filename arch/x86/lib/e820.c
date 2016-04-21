#include "arch/x86/e820.h"

#include <stdio.h>
#include <arch/x86/bios.h>


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
               "\tbase address: %#x%08x\r\n" \
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

#ifdef BOOTLOADER_PROTECTED_MODE_ENABLED
    (void) buffer;
    (void) nb_max_entries;
    (void) continuation_val;
    (void) signature;
    (void) bytes;
    // TODO
#else
    x86_regs_t regs_in, regs_out;

    x86_regs_init(&regs_in);
    regs_in.ES = regs_in.DS;

    do {
        regs_in.B._eRx = continuation_val;
        regs_in.C._eRx = sizeof(SMAP_entry_t);
        regs_in.D._eRx = 0x534D4150;
        regs_in.DI._R = (uint32_t)buffer;

        regs_in.A._Rx = 0xe820;
        bioscall(0x15, &regs_in, &regs_out);

        signature = regs_out.A._eRx;
        continuation_val = regs_out.B._eRx;
        bytes = regs_out.C._eRx;

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
#endif

    return nb_entries;
}
