#include "arch/x86/pit.h"

#include <arch/x86/ioport.h>
#include <assert.h>
#include <stdio.h>

#define PIT_CLK_RATE 1193181 // ~1.193MHz

#define PIT_REG_COUNTER0 0x40
#define PIT_REG_COUNTER1 0x41
#define PIT_REG_COUNTER2 0x42
#define PIT_REG_COMMAND  0x43

// Control word register's masks
#define PIT_MASK_BINCOUNTER 0x01 //00000001
#define PIT_MASK_OPMODE     0x0E //00001110
#define PIT_MASK_RLMODE     0x30 //00110000
#define PIT_MASK_COUNTER    0xC0 //11000000

// Binary counter type
#define PIT_BINCOUNTER_BINARY 0
#define PIT_BINCOUNTER_BCD    1 // Binary Coded Decimal

// Operating mode
#define PIT_OPMODE_TERMINALCOUNT       (0 << 1)
#define PIT_OPMODE_PROGRAMMABLEONESHOT (1 << 1)
#define PIT_OPMODE_RATEGENERATOR       (2 << 1)
#define PIT_OPMODE_SQUAREWAVEGENERATOR (3 << 1)
#define PIT_OPMODE_SOFTWARETRIGSTROBE  (4 << 1)
#define PIT_OPMODE_HARDWARETRIGSTROBE  (5 << 1)

// Read/Load mode
#define PIT_RLMODE_LATCH   (0 << 4)
#define PIT_RLMODE_LSBONLY (1 << 4)
#define PIT_RLMODE_MSBONLY (2 << 4)
#define PIT_RLMODE_DATA    (3 << 4)

// Counters
#define PIT_COUNTER_0 (0 << 6)
#define PIT_COUNTER_1 (1 << 6)
#define PIT_COUNTER_2 (2 << 6)

#define pit_set_bincounter(ocw, binmode) \
	( ((ocw) & ~PIT_MASK_BINCOUNTER) | (binmode) )

#define pit_set_opmode(ocw, opmode) \
	( ((ocw) & ~PIT_MASK_OPMODE) | ((opmode) << 1) )

#define pit_set_rlmode(ocw, rlmode) \
	( ((ocw) & ~PIT_MASK_RLMODE) | ((rlmode) << 4) )

#define pit_set_counter(ocw, counter) \
	( ((ocw) & ~PIT_MASK_COUNTER) | ((counter) << 6) )

void i86_pit_start_counter (uint32_t val, enum pit_config_val_t type_val, pit_mode_t mode)
{
    assert_if(type_val == FREQUENCY,
            val != 0 && val <= PIT_CLK_RATE);

    uint16_t divisor = (type_val == FREQUENCY) ? PIT_CLK_RATE / val : val;

    uint32_t freq = (type_val == FREQUENCY) ? val : PIT_CLK_RATE / divisor;
    debug_printf("PIT: counter %u --> frequency = %uHz, divisor=%u\r\n", mode.counter, freq, divisor);

    uint8_t ocw = 0;
    ocw = pit_set_bincounter(ocw, PIT_BINCOUNTER_BINARY);
    ocw = pit_set_opmode(ocw, mode.opmode);
    ocw = pit_set_rlmode(ocw, PIT_RLMODE_DATA);
    ocw = pit_set_counter(ocw, mode.counter);
    outb(PIT_REG_COMMAND, ocw);

    uint16_t pit_counter_reg = 0x40 + mode.counter;

    // set frequency rate
    // COUNT = input(Hz) / Frequency of output
    outb(pit_counter_reg, divisor & 0x00ff);
    outb(pit_counter_reg, (divisor >> 8) & 0x00ff);
}
