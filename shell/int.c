#include "shell_cmds.h"

#include <string.h>
#include <ctype.h>
#include <sys/hal.h>
#include <stdio.h>

bool arg_checkdigits(const char* str, uint8_t base)
{
    bool valid = true;
    size_t len = strlen(str);

    for (size_t i=2; valid && i < len; ++i) {
        valid &= isdigitinbase(str[i], base);
    }

    return valid;
}

int32_t do_int(uint8_t argc, char** argv)
{
    (void) argc;
    (void) argv;

    if (argc != 2) {
        return EXIT_FAILURE;
    }

    const char* str = argv[1];
    if (!isdigit(str[0])) {
        return EXIT_FAILURE;
    }

    uint8_t intno;
    uint8_t base = 10;

    if ( strncmp("0x", str, 2) == 0 ) {
        base = 16;
        str += 2;
    }

    if ( !arg_checkdigits(str, base) ) {
        return EXIT_FAILURE;
    }

    intno = strtoull(str, NULL, base);

    hal_geninterrupt(intno);

    return EXIT_SUCCESS;
}
