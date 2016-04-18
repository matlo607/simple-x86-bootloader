#include <sys/tty.h>

#include <arch/x86/bios.h>

void tty_initialize(void)
{

}

void tty_put_char(char c)
{
    //__asm__ __volatile__ ("movb %[bios_service_print_char_active_page], %%ah;"
    //                      "movb %[char_to_print], %%al;"
    //                      "movb %[color], %%bl;"
    //                      "int $0x10;"
    //                      :
    //                      : [bios_service_print_char_active_page] "i" (0x0e),
    //                      [char_to_print] "o" (c),
    //                      [color] "i" (0x07)
    //                      : "%eax", "%ebx"
    //                      );

    x86_regs_t regs_in, regs_out;

    x86_regs_init(&regs_in);

    regs_in.A._Rl = c;
    regs_in.B._Rl = 0x07;
    regs_in.A._Rh = 0x0e;

    bioscall(0x10, &regs_in, &regs_out);
}

size_t tty_write(const char* buf, size_t size)
{
    (void) buf;
    (void) size;

    return 0;
}
