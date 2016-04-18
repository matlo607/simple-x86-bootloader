#include "arch/x86/e801.h"

#include <stdio.h>
#include <arch/x86/bios.h>

bool e801_detect_upper_mem(e801_upper_mem_t* mem_properties)
{
    bool succeeded = false;
    x86_regs_t regs_in, regs_out;

    x86_regs_init(&regs_in);

    /*
     * CF    Carry Flag    Non-Carry - indicates no error
     * %ax   Extended 1    Number of contiguous KB between 1 and 16 MB, maximum 0x3C00 = 15 MB.
     * %bx   Extended 2    Number of contiguous 64 KB blocks between 16 MB and 4 GB.
     * %cx   Configured 1  Number of contiguous KB between 1 and 16 MB, maximum 0x3C00 = 15 MB.
     * %dx   Configured 2  Number of contiguous 64 KB blocks between 16 MB and 4 GB.
     * Some BIOSes always return with %ax = %bx = 0, others with %cx = %dx = 0, and others
     * returns valid results in both. We have to perform a check on these returned values.
     **/
    regs_in.A._Rx = 0xe801;
    bioscall(0x15, &regs_in, &regs_out);

    if (regs_out.eflags.CF) {
        if (regs_out.A._Rh == 0x86) {
            printf("int 0x15, ax=0xE801: unsupported function\r\n");
        } else if (regs_out.A._Rh == 0x80) {
            printf("int 0x15, ax=0xE801: invalid command\r\n");
        }

    } else {

        if (regs_out.A._Rx || regs_out.B._Rx) {
            mem_properties->_1MB_to_16MB = regs_out.A._Rx;
            mem_properties->_16MB_to_4GB = regs_out.B._Rx;
        } else  {
            mem_properties->_1MB_to_16MB = regs_out.C._Rx;
            mem_properties->_16MB_to_4GB = regs_out.D._Rx;
        }

        if (mem_properties->_1MB_to_16MB <= 0x3c00) { // Maximum 15MB [0x3c00]
            succeeded = true;
        }

        mem_properties->_16MB_to_4GB = mem_properties->_16MB_to_4GB * 64;
    }

    return succeeded;
}
