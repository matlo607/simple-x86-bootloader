#include <sys/input.h>

#include <arch/x86/bios.h>

void input_initialize(void)
{

}

char input_get_char(void)
{
    char c;

    //__asm__ __volatile__("movb %[bios_service_read_buffer], %%ah;"
    //                     "int $0x16;"
    //                     : "=a" (c)
    //                     : [bios_service_read_buffer] "i" (0x00)
    //                     : "cc"
    //                     );

    x86_regs_t regs_in, regs_out;

    x86_regs_init(&regs_in);

    regs_in.A._Rh = 0x00;

    bioscall(0x16, &regs_in, &regs_out);

    c = regs_out.A._Rl;

    return c;
}

size_t input_read(char* buf, size_t size)
{
    (void) buf;
    (void) size;

    return 0;
}
