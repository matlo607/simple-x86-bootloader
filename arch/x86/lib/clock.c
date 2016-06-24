#include "arch/x86/clock.h"

#include <arch/x86/idt.h>
#include <arch/x86/pic.h>
#include <arch/x86/pit.h>
#include <sys/time.h>
#include <string.h>
#include <stdio.h>

#define SYS_CLOCK_FREQ 100 // 100Hz --> 10ms
#define _1s_to_ns 1000000000

#define SYS_CLOCK_RESOLUTION_TIC_ms 10
#define SYS_CLOCK_RESOLUTION_TIC_us ((SYS_CLOCK_RESOLUTION_TIC_ms) * 1000)
#define SYS_CLOCK_RESOLUTION_TIC_ns ((SYS_CLOCK_RESOLUTION_TIC_us) * 1000)
#define SYS_CLOCK_nbTICS_1s         (_1s_to_ns / (SYS_CLOCK_RESOLUTION_TIC_ns))


static volatile uint64_t clk_tics;

static volatile struct timespec sysclock;

// interrupt handler's assembly wrapper
extern void _asm_clock_int(void);

void i86_clock_init(void)
{
    clk_tics = 0;
    sysclock.tv_sec = 0;
    sysclock.tv_nsec = 0;

    // set up PIT to give the wanted frequency
    pit_mode_t mode = { .opmode = SQUARE_WAVE_GENERATOR, .counter = COUNTER_0 };
    i86_pit_start_counter(SYS_CLOCK_FREQ, FREQUENCY, mode);

    // install clock's ISR in IDT
    idt_descriptor_flags_t flags;
    memset(&flags, 0, sizeof(idt_descriptor_flags_t));

    flags.gate_type = IDT_DESC_TYPE_32BIT_INTERRUPT_GATE;
    flags.privilege_level = IDT_DESC_PRIVILEGE_RING_0;
    flags.present = IDT_DESC_PRESENT_IN_MEMORY;

    i86_idt_install_ir(IDT32_HARDWARE_INT_OFFSET(CLOCKINT), _asm_clock_int, flags);

    // unmask clock's interrupt
    uint16_t intmask = i86_pic_getintmask();
    intmask = mask_enableinterrupt(intmask, CLOCKINT);
    i86_pic_setintmask(intmask);
}

void clock_getresolution(struct timespec* res)
{
    res->tv_sec = 0;
    res->tv_nsec = SYS_CLOCK_RESOLUTION_TIC_ns;
}

void clock_gettime(struct timespec* timestamp)
{
    timestamp->tv_sec = sysclock.tv_sec;
    timestamp->tv_nsec = sysclock.tv_nsec;
}

#include "timer.c.h"

// interrupt handler
void isr_clock_int(void)
{
    ++clk_tics;

    sysclock.tv_nsec += SYS_CLOCK_RESOLUTION_TIC_ns;

    if (sysclock.tv_nsec % _1s_to_ns == 0) {
        ++sysclock.tv_sec;
        sysclock.tv_nsec = 0;
        //debug_printf("sysclock: %lu.%09lu\r\n", sysclock.tv_sec, sysclock.tv_nsec);
    }

    // defined in timer.c.h
    if (!softtimer0.stop) {
        softtimer0.spent.tv_nsec += SYS_CLOCK_RESOLUTION_TIC_ns;

        if (softtimer0.spent.tv_nsec % _1s_to_ns == 0) {
            ++softtimer0.spent.tv_sec;
            softtimer0.spent.tv_nsec = 0;
            //debug_printf("softtimer0: %lu.%09lu\r\n", softtimer0.spent.tv_sec, softtimer0.spent.tv_nsec);
        }
    }
}
