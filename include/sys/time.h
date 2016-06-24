#ifndef _SYS_TIME_H
#define _SYS_TIME_H

#include <stdint.h>

typedef uint64_t time_t;

struct timespec {
    time_t   tv_sec;  /* seconds */
    uint64_t tv_nsec; /* nanoseconds */
};

extern void clock_getresolution(struct timespec* res);
extern void clock_gettime(struct timespec* timestamp);

void sleep(time_t seconds);
void nanosleep(const struct timespec* duration);

#endif
