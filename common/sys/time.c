#include "sys/time.h"

#include <sys/timer.h>
#include <stddef.h>

void sleep(time_t seconds)
{
    struct timespec wait = { .tv_sec = seconds, .tv_nsec = 0 };
    timer_wind(ONE_SHOT, &wait, NULL);
    timer_start();
}

void nanosleep(const struct timespec* duration)
{
    timer_wind(ONE_SHOT, duration, NULL);
    timer_start();
}
