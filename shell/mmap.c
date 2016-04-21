#include "shell_cmds.h"

#include <stdio.h>

#include <arch/x86/e820.h>

int32_t do_mmap(uint8_t argc, char** argv)
{
    (void) argc;
    (void) argv;

    SMAP_entry_t e820_smap[20];
    ssize_t e820_smap_nb_entries = e820_detect_mem(e820_smap, 20);

    if (e820_smap_nb_entries == -1) {
        printf("int 0x15, 0xe820 : error while mapping memory\r\n");
        return EXIT_FAILURE;
    }

    printf("Memory map (nb entries = %d)\r\n", e820_smap_nb_entries);
    e820_display_SMAP_entries(e820_smap, e820_smap_nb_entries);

    return EXIT_SUCCESS;
}
