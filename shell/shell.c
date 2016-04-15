#include "shell.h"

#include "shell_cmds.h"
#include "io.h"
#include "string.h"


#define SHELL_LINE_MAX_LEN 100
#define SHELL_ARGS_MAX_NB  10


static uint8_t shell_split_line(char* line, char* argv[])
{
    uint8_t argc = 1;

#ifdef DEBUG_SHELL
    char** argv_start = argv;
#endif

    *argv = line;
    argv++;

    while (line != NULL && argc <= SHELL_ARGS_MAX_NB) {

        char* ptr_next_arg = strchr(line, ' ');

        if (ptr_next_arg == NULL) {
            break;
        } else {

            // mark the end of the previous argument
            *ptr_next_arg = '\0';
            ptr_next_arg++;

            // discard space until foundind another character
            while (*ptr_next_arg == ' ') {
                ptr_next_arg++;
            }

            if (*ptr_next_arg == '\0') {
                break;
            }

            *argv = ptr_next_arg;
            argv++;
            argc++;

            line = ptr_next_arg;
        }
    }

#ifdef DEBUG_SHELL
    for (uint8_t i = 0; i < argc; ++i) {
        debug_printf("\targ %u : \"%s\"\r\n", i, argv_start[i]);
    }
#endif

    return argc;
}

static int32_t shell_execute(uint8_t argc, char* argv[])
{
    size_t i = 0;
    int32_t status = -1;

    while (i < COMMAND_NUMBER) {

        if (strcmp(argv[0], cmd_table[i].name) == 0) {
            // command found, execute it

#ifdef DEBUG_SHELL
            debug_printf("command %s found at position %u, execute it !\r\n", argv[0], i);
#endif
            status = (*cmd_table[i].func_cmd)(argc, argv);
#ifdef DEBUG_SHELL
            debug_printf("command %s returned status %d\r\n", argv[0], status);
#endif
            break;
        }

        i++;
    }

    if (i >= COMMAND_NUMBER) {
        printf("error: unknown command %s\r\n", argv[0]);
    }

    return status;
}

void shell_init()
{

}

void shell_main()
{
    char line[SHELL_LINE_MAX_LEN + 1] = { '\0' };

    puts(SHELL_PROMPT "/> ");    

    size_t n = getline(line, SHELL_LINE_MAX_LEN+1); 
    line[n-1] = '\0'; // remove '\r'

#ifdef DEBUG_SHELL
    debug_printf("read %u characters from command line\r\n", n);
    debug_printf("\tline = \"%s\"\r\n", line);
#endif

    if (n > 1) {
        char* argv[SHELL_ARGS_MAX_NB] = { NULL };

        uint8_t argc = shell_split_line(line, argv);

        shell_execute(argc, argv);
    }
}
