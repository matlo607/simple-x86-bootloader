#ifndef _SHELL_CMDS_H
#define _SHELL_CMDS_H

#include "common.h"

typedef struct shell_command_s {

    const char* name;
    const char* description;
    int32_t (*func_cmd)(uint8_t argc, char** argv);

} shell_command_t;

// commands list
extern int32_t shell_cmd_help(uint8_t argc, char** argv);


// to update everytime we add/remove a command
#define COMMAND_NUMBER  1
static const shell_command_t cmd_table[COMMAND_NUMBER] =
{
    {
        .name = "help",
        .description = "display a list of all the commands and their description",
        .func_cmd = &shell_cmd_help
    }
};

#endif
