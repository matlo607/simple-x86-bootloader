#include "memory.h"

#include "screen.h"

int16_t memory_map_int15_E820(SMAP_entry_t* buffer, int16_t nb_max_entries)
{
    uint32_t continuation_val = 0;
    uint32_t signature;
    uint32_t bytes = 0;

    int16_t nb_entries = 0;

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
                              : [signature_out] "=g" (signature),
                                [continuation_val_out] "=g" (continuation_val),
                                [bytes] "=g" (bytes)
                              : [bios_service_e820] "i" (0xe820),
                                [continuation_val_in] "g" (continuation_val),
                                [struct_size] "i" (24),
                                [signature_in] "i" (0x534D4150),
                                [buffer] "g" (buffer)
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

bool upper_memory_properties_int15_E801(upper_memory_prop_t* properties);

#if 0
bool upper_memory_properties_int15_88(upper_memory_prop_t* properties);
#endif

int16_t lower_memory_properties(void)
{
    int16_t lower_mem = 0;
    bool res = false;

    // Get the size of lower memory (memory before the EBDA (Extended BIOS Data Area) but
    // including also the real mode IVT (Interrupt Vector Table) and the BDA (BIOS Data
    // Area).
    //
    // In the lower memory (< 1MB), the memory guaranteed free for use lies between
    // the BDA (BIOS Data Area) and the EBDA (Extended BIOS Data Area).
    // The amount of memory (in kB) returned by the BIOS interrupt 0x12 in %ax is the
    // amount of RAM (just considering the lower memory) determined by POST minus the
    // memory allocated for the EBDA and the area allocated for the video memory and
    // BIOS rom.
    // Thus we have to substract from this amount the IVT and the BDA (0x500 bytes)
    __asm__ __volatile__ ("clc;"
                          "int $0x12;"
                          "jc 1f;"
                          "movw %%ax, %[lower_mem];"
                          "movb $1, %[res];"
                          "1:"
                          : [lower_mem] "=g" (lower_mem), // number of contiguous 1kB blocks
                                                          // of memory before EBDA
                            [res] "=g" (res)
                          :
                          : "%eax", "cc");

    if (!res) {
        lower_mem = -1;
    }

    // available memory = lower_mem * 1024 - 0x500
    return lower_mem;
}

bool upper_memory_properties(upper_memory_prop_t* properties)
{
    bool succeeded = false;

    // The region of RAM above 1 MiB is not standardized, well-defined, or contiguous.
    // Some sub-regions of it contains memory mapped hardware, that nothing but a device
    // driver should ever access.
    // Area from : 0x00100000 to 0x00EFFFFF (14MB) -- free for use (if it exists)
    //             0x00F00000 to 0x00FFFFFF (1MB)  -- Possible memory mapped hardware
    //             0x00F00000 to 0x00FFFFFF (1MB)  -- ISA Memory Hole 15-16MB
    //             0x01000000 to ????????   (?MB)  -- free for use (if it exists)
    // When we go beyond 16MB, the best way to detect the memory of a PC
    // --> INT 0x15, %ax = 0xE801 command


    // We will use INT 0x15, %ax = 0xE801 command instead.
    succeeded = upper_memory_properties_int15_E801(properties);
#if 0
    if (!succeeded) {
        // ultimate hope before abandon (does not work on Qemu, damn it !)
        succeeded = upper_memory_properties_int15_88(properties);
    }
#endif

    return succeeded;
}

/*
 * CF    Carry Flag    Non-Carry - indicates no error
 * %ax   Extended 1    Number of contiguous KB between 1 and 16 MB, maximum 0x3C00 = 15 MB.
 * %bx   Extended 2    Number of contiguous 64 KB blocks between 16 MB and 4 GB.
 * %cx   Configured 1  Number of contiguous KB between 1 and 16 MB, maximum 0x3C00 = 15 MB.
 * %dx   Configured 2  Number of contiguous 64 KB blocks between 16 MB and 4 GB.
 * Some BIOSes always return with %ax = %bx = 0, others with %cx = %dx = 0, and others
 * returns valid results in both. We have to perform a check on these returned values.
 **/
bool upper_memory_properties_int15_E801(upper_memory_prop_t* properties)
{
    bool succeeded = false;

    uint8_t cmd_status = 0;
    uint16_t upper_mem_1MB_to_16MB_extended = 0;
    uint16_t upper_mem_16MB_to_4GB_extended = 0;
    uint16_t upper_mem_1MB_to_16MB_configured = 0;
    uint16_t upper_mem_16MB_to_4GB_configured = 0;

    __asm__ __volatile__ ("push %%edx;"
                          "clc;"
                          "movw %[bios_service_upper_mem], %%ax;"
                          "xorw %%bx, %%bx;"
                          "xorw %%cx, %%cx;"
                          "xorw %%dx, %%dx;"
                          "int $0x15;"
                          //"jc 1f;"
                          "movb %%ah, %[cmd_status];"
                          "movw %%ax, %[upper_mem_1MB_to_16MB_extended];"
                          "movw %%bx, %[upper_mem_16MB_to_4GB_extended];"
                          "movw %%cx, %[upper_mem_1MB_to_16MB_configured];"
                          "movw %%dx, %[upper_mem_16MB_to_4GB_configured];"
                          //"movb $1, %[res];"
                          "1: pop %%edx;"
                          : [cmd_status] "=g" (cmd_status),
                            [upper_mem_1MB_to_16MB_extended] "=g" (upper_mem_1MB_to_16MB_extended),
                            [upper_mem_16MB_to_4GB_extended] "=g" (upper_mem_16MB_to_4GB_extended),
                            [upper_mem_1MB_to_16MB_configured] "=g" (upper_mem_1MB_to_16MB_configured),
                            [upper_mem_16MB_to_4GB_configured] "=g" (upper_mem_16MB_to_4GB_configured)//,
                            //[res] "=g" (succeeded)
                          : [bios_service_upper_mem] "i" (0xe801)
                          : "%eax", "%ebx", "%ecx", "cc");

    //printf("cmd_status : 0x%x\r\n", cmd_status);
    if (cmd_status == 0x86) {
        printf("int 0x15, ax=0xE801: unsupported function\r\n");

    } else if (cmd_status == 0x80) {
        printf("int 0x15, ax=0xE801: invalid command\r\n");

    } else {
        //printf("Upper memory extended: %uKB [0x%x] (maximum 15MB [0x3c00])\r\n",
        //        upper_mem_1MB_to_16MB_extended,
        //        upper_mem_1MB_to_16MB_extended);
        //printf("                       %u x 64KB [0x%x]\r\n",
        //        upper_mem_16MB_to_4GB_extended,
        //        upper_mem_16MB_to_4GB_extended);

        //printf("Upper memory configured: %uKB [0x%x] (maximum 15MB [0x3c00])\r\n",
        //        upper_mem_1MB_to_16MB_configured,
        //        upper_mem_1MB_to_16MB_configured);
        //printf("                         %u x 64KB [0x%x]\r\n",
        //        upper_mem_16MB_to_4GB_configured,
        //        upper_mem_16MB_to_4GB_configured);

        if (upper_mem_1MB_to_16MB_extended || upper_mem_16MB_to_4GB_extended) {
            properties->_1MB_to_16MB = upper_mem_1MB_to_16MB_extended;
            properties->_16MB_to_4GB = upper_mem_16MB_to_4GB_extended;
        } else if (upper_mem_1MB_to_16MB_extended || upper_mem_16MB_to_4GB_extended) {
            properties->_1MB_to_16MB = upper_mem_1MB_to_16MB_configured;
            properties->_16MB_to_4GB = upper_mem_16MB_to_4GB_configured;
        } else {
            properties->_1MB_to_16MB = 0;
            properties->_16MB_to_4GB = 0;
        }

        if (properties->_1MB_to_16MB <= 0x3c00) { // Maximum 15MB [0x3c00]
            succeeded = true;
        }

        properties->_16MB_to_4GB = properties->_16MB_to_4GB * 64;
    }

    return succeeded;
}

#if 0
bool upper_memory_properties_int15_88(upper_memory_prop_t* properties)
{
    bool succeeded = false;
    uint8_t cmd_status = 0;

    __asm__ __volatile__ (
                          //"clc;"
                          "movb %[bios_service_upper_mem], %%ah;"
                          "int $0x15;"
                          //"jc 1f;"
                          //"test %%ax, %%ax;"
                          //"je 1f;"
                          "movb %%ah, %[cmd_status];"
                          "movw %%ax, %[upper_mem_1MB_to_16MB];"
                          "movb $1, %[res];"
                          //"1:"
                          : [res] "=g" (succeeded),
                            [cmd_status] "=g" (cmd_status),
                            [upper_mem_1MB_to_16MB] "=g" (properties->_1MB_to_16MB)
                          : [bios_service_upper_mem] "i" (0x88)
                          : "%eax", "cc"
                          );

    // Set it to 0 but does not mean there is not memory above 15MB
    properties->_16MB_to_4GB = 0;

    if (cmd_status == 0x86) {
        printf("int 0x15, ax=0x88: unsupported function\r\n");

    } else if (cmd_status == 0x80) {
        printf("int 0x15, ax=0x88: invalid command\r\n");

    } else {
        succeeded = true;
    }

    if (!succeeded) {
        properties->_1MB_to_16MB = 0;
    }

    return succeeded;
}
#endif
