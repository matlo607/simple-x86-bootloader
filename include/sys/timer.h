#ifndef _TIMER_H
#define _TIMER_H

#include <sys/time.h>

typedef enum {
    ONE_SHOT,
    AUTO_REWIND
} timer_feature_t;

typedef void (*timer_trigger_handler_t) (void);

void timer_wind(timer_feature_t type, const struct timespec* wait, timer_trigger_handler_t handler);

void timer_start(void);

void timer_disarm(void);

void timer_rewind(void);

#endif
