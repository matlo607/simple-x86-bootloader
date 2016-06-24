#include <sys/input.h>

#ifdef PM_DRIVERS

#include <string.h>
#include <stdbool.h>

#include <arch/x86/cpu.h>
#include <arch/x86/idt.h>

extern void _asm_kbd_getinput_int(void);

volatile char driver_kbd_inputc = '\0';
volatile bool driver_kbd_inputavailable = false;

#else

#include <arch/x86/bios.h>

#endif

void input_initialize(void)
{
#ifdef PM_DRIVERS
    driver_kbd_inputavailable = false;

    idt_descriptor_flags_t flags;
    memset(&flags, 0, sizeof(idt_descriptor_flags_t));

    flags.gate_type = IDT_DESC_TYPE_32BIT_INTERRUPT_GATE;
    flags.privilege_level = IDT_DESC_PRIVILEGE_RING_0;
    flags.present = IDT_DESC_PRESENT_IN_MEMORY;

    i86_idt_install_ir(IDT32_SOFTWARE_INT_OFFSET(KBDINPUTINT), _asm_kbd_getinput_int, flags);
#endif
}

char input_get_char(void)
{
    char c;

#ifdef PM_DRIVERS
    __asm__ __volatile ("int %[intno];"
            :
            : [intno] "i" (IDT32_SOFTWARE_INT_OFFSET(KBDINPUTINT))
            :);
    c = driver_kbd_inputc;
    driver_kbd_inputavailable = false;
#else

#ifdef BOOTLOADER_PROTECTED_MODE_ENABLED
    regs16_t regs;

    regs.a._Rh = 0x00;

    int32(0x16, &regs);

    c = regs.a._Rl;
#else
    x86_regs_t regs_in, regs_out;

    x86_regs_init(&regs_in);

    regs_in.A._Rh = 0x00;

    bioscall(0x16, &regs_in, &regs_out);

    c = regs_out.A._Rl;
#endif

#endif

    return c;
}

size_t input_read(char* buf, size_t size)
{
    (void) buf;
    (void) size;

    return 0;
}

#ifdef PM_DRIVERS
void isr_kbd_getinput_int(void)
{
    /* By default, interrupts are disabled when we enter in an ISR.
     * We have to re-enable them again to allow the system clock to work.
     */
    sti();

    while (!driver_kbd_inputavailable) {
        hlt();
    }
}
#endif
