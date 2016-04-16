#include "shell_cmds.h"

#include <stddef.h>
#include <sys/system.h>

int32_t do_reboot(uint8_t argc, char** argv)
{
    (void) argc;
    (void) argv;

    reboot();

    return EXIT_SUCCESS;
}
