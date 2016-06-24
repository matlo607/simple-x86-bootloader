#include "sys/timer.h"

#include <arch/x86/cpu.h>

#include <stdbool.h>

typedef struct {
    struct timespec wait;
    volatile struct timespec spent;
    timer_feature_t type;
    timer_trigger_handler_t handler;
    volatile bool stop;
} software_time_t;

static software_time_t softtimer0;

extern void _asm_timer_int(void);

void i86_softwaretimer_init(void)
{
    idt_descriptor_flags_t flags;
    memset(&flags, 0, sizeof(idt_descriptor_flags_t));

    flags.gate_type = IDT_DESC_TYPE_32BIT_INTERRUPT_GATE;
    flags.privilege_level = IDT_DESC_PRIVILEGE_RING_0;
    flags.present = IDT_DESC_PRESENT_IN_MEMORY;

    i86_idt_install_ir(IDT32_SOFTWARE_INT_OFFSET(TIMERINT), _asm_timer_int, flags);
}

void timer_wind(timer_feature_t type, const struct timespec* wait, timer_trigger_handler_t handler)
{
    softtimer0.stop = true;
    softtimer0.spent.tv_sec = 0;
    softtimer0.spent.tv_nsec = 0;

    softtimer0.type = type;
    memcpy(&softtimer0.wait, wait, sizeof(struct timespec));
    softtimer0.handler = handler;
}

void timer_start(void)
{
    softtimer0.spent.tv_sec = 0;
    softtimer0.spent.tv_nsec = 0;
    softtimer0.stop = false;

    __asm__ __volatile ("int %[intno];"
            :
            : [intno] "i" (IDT32_SOFTWARE_INT_OFFSET(TIMERINT))
            :);
}

void timer_disarm(void)
{
    softtimer0.stop = true;
}

void timer_rewind(void)
{
    softtimer0.spent.tv_sec = 0;
    softtimer0.spent.tv_nsec = 0;

    softtimer0.stop = false;
}

void isr_timer_int(void)
{
    /* By default, interrupts are disabled when we enter in an ISR.
     * We have to re-enable them again to allow the system clock to work.
     */
    sti();

    //debug_printf("TIMER(duration:%lu.%09lus, mode=%s): launch\r\n",
    //        softtimer0.wait.tv_sec, softtimer0.wait.tv_nsec,
    //        (softtimer0.type == AUTO_REWIND) ? "auto-rewind": "oneshot");

    do {
        while (!softtimer0.stop
                && (softtimer0.wait.tv_sec > softtimer0.spent.tv_sec
                    || (softtimer0.wait.tv_sec == softtimer0.spent.tv_sec
                        && softtimer0.wait.tv_nsec > softtimer0.spent.tv_nsec)
                   )
              ) {
            hlt();
        }

        timer_disarm();

        //debug_printf("TIMER: expired, execute handler\r\n");

        (*softtimer0.handler)();

        if (softtimer0.type == AUTO_REWIND) {
            timer_rewind();
        }

    } while (!softtimer0.stop && softtimer0.type == AUTO_REWIND);

    //debug_printf("TIMER: disabled, softtimer0.stop=%u\r\n", softtimer0.stop);
}
