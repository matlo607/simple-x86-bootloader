#include "shell_cmds.h"

#include "io.h"

int32_t shell_cmd_help(uint8_t argc, char** argv)
{
    (void) argc;
    (void) argv;

    for (size_t i = 0; i < COMMAND_NUMBER; ++i) {

        printf("%s\r\n", cmd_table[i].name);
        printf("\t%s\r\n", cmd_table[i].description);
    }

    return EXIT_SUCCESS;
}
