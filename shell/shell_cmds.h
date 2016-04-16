#ifndef _SHELL_CMDS_H
#define _SHELL_CMDS_H

#include <stdint.h>

typedef struct shell_command_s {

    const char* name;
    const char* description;
    int32_t (*func_cmd)(uint8_t argc, char** argv);

} shell_command_t;

// commands list
extern int32_t do_help(uint8_t argc, char** argv);
extern int32_t do_reboot(uint8_t argc, char** argv);


// to update everytime we add/remove a command
#define COMMAND_NUMBER 2
static const shell_command_t cmd_table[COMMAND_NUMBER] =
{
    {
        .name = "help",
        .description = "display a list of all the commands and their description",
        .func_cmd = &do_help
    },
    {
        .name = "reboot",
        .description = "reboot the system",
        .func_cmd = &do_reboot
    }
};

#endif
