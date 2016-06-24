#ifndef _PIT_H
#define _PIT_H

#include <stdint.h>

enum pit_counterno_t {
    COUNTER_0 = 0,
    COUNTER_1 = 1,
    COUNTER_2 = 2
};

enum pit_opmode_t {
    TERMINAL_COUNT = 0,
    PROGRAMMABLE_ONESHOT = 1,
    RATE_GENERATOR = 2,
    SQUARE_WAVE_GENERATOR = 3,
    SOFTWARE_TRIGGERED_STROBE = 4,
    HARDWARE_TRIGGERED_STROBE = 5
};

typedef struct {
    uint8_t reserved_0 : 1;
    enum pit_opmode_t opmode : 3;
    uint8_t reserved_4_5 : 2;
    enum pit_counterno_t counter: 2;
} pit_mode_t;

enum pit_config_val_t {
    FREQUENCY,
    DIVISOR
};

void i86_pit_start_counter(uint32_t val, enum pit_config_val_t type_val, pit_mode_t mode);

#endif
