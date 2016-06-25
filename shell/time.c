#include "shell_cmds.h"

#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include <arch/x86/e820.h>

int32_t do_time(uint8_t argc, char** argv)
{
    (void) argc;
    (void) argv;

    struct timespec current;
    memset(&current, 0, sizeof(struct timespec));

    clock_gettime(&current);

    printf("time: %lu.%09lu seconds\r\n", current.tv_sec, current.tv_nsec);

    return EXIT_SUCCESS;
}
