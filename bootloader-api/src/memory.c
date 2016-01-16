#include "memory.h"

#include "screen.h"

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
                          : "cc");

    if (!res) {
        lower_mem = -1;
    }

    // available memory = lower_mem * 1024 - 0x500
    return lower_mem;
}

bool upper_memory_properties(upper_memory_prop_t* properties)
{
    bool succeeded = false;

    uint16_t upper_mem_1MB_to_16MB_extended = 0;
    uint16_t upper_mem_16MB_to_4GB_extended = 0;
    uint16_t upper_mem_1MB_to_16MB_configured = 0;
    uint16_t upper_mem_16MB_to_4GB_configured = 0;

    // The region of RAM above 1 MiB is not standardized, well-defined, or contiguous.
    // Some sub-regions of it contains memory mapped hardware, that nothing but a device
    // driver should ever access.
    // Area from : 0x00100000 to 0x00EFFFFF (14MB) -- free for use (if it exists)
    //             0x00F00000 to 0x00FFFFFF (1MB)  -- Possible memory mapped hardware
    //             0x00F00000 to 0x00FFFFFF (1MB)  -- ISA Memory Hole 15-16MB
    //             0x01000000 to ????????   (?MB)  -- free for use (if it exists)
    // When we go beyond 16MB, the best way to detect the memory of a PC
    // --> INT 0x15, %ax = 0xE820 command
    // But quite complicated to handle for us since we just need some MBs to load the kernel
    // but for sure less than 16MB.
    // We will use INT 0x15, %ax = 0xE801 command instead.
    // CF    Carry Flag    Non-Carry - indicates no error
    // %ax   Extended 1    Number of contiguous KB between 1 and 16 MB, maximum 0x3C00 = 15 MB.
    // %bx   Extended 2    Number of contiguous 64 KB blocks between 16 MB and 4 GB.
    // %cx   Configured 1  Number of contiguous KB between 1 and 16 MB, maximum 0x3C00 = 15 MB.
    // %dx   Configured 2  Number of contiguous 64 KB blocks between 16 MB and 4 GB.
    // Some BIOSes always return with %ax = %bx = 0, others with %cx = %dx = 0, and others
    // returns valid results in both. We have to perform a check on these returned values.

    __asm__ __volatile__ ("clc;"
                          "movw %[bios_service_upper_mem], %%ax;"
                          "xorw %%bx, %%bx;"
                          "xorw %%cx, %%cx;"
                          "xorw %%dx, %%dx;"
                          "int $0x15;"
                          "jc 1f;"
                          "movw %%ax, %[upper_mem_1MB_to_16MB_extended];"
                          "movw %%bx, %[upper_mem_16MB_to_4GB_extended];"
                          "movw %%cx, %[upper_mem_1MB_to_16MB_configured];"
                          "movw %%dx, %[upper_mem_16MB_to_4GB_configured];"
                          "movb $1, %[res];"
                          "1:"
                          : [upper_mem_1MB_to_16MB_extended] "=g" (upper_mem_1MB_to_16MB_extended),
                            [upper_mem_16MB_to_4GB_extended] "=g" (upper_mem_16MB_to_4GB_extended),
                            [upper_mem_1MB_to_16MB_configured] "=g" (upper_mem_1MB_to_16MB_configured),
                            [upper_mem_16MB_to_4GB_configured] "=g" (upper_mem_16MB_to_4GB_configured),
                            [res] "=g" (succeeded)
                          : [bios_service_upper_mem] "i" (0xe820)
                          : "cc");

    printf("Upper memory extended: %uKB [0x%x] (maximum 15MB [0x3c00])\r\n",
            upper_mem_1MB_to_16MB_extended,
            upper_mem_1MB_to_16MB_extended);
    printf("                       %u x 64KB [0x%x]\r\n",
            upper_mem_16MB_to_4GB_extended,
            upper_mem_16MB_to_4GB_extended);

    printf("Upper memory configured: %uKB [0x%x] (maximum 15MB [0x3c00])\r\n",
            upper_mem_1MB_to_16MB_configured,
            upper_mem_1MB_to_16MB_configured);
    printf("                         %u x 64KB [0x%x]\r\n",
            upper_mem_16MB_to_4GB_configured,
            upper_mem_16MB_to_4GB_configured);

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

    return succeeded;
}

