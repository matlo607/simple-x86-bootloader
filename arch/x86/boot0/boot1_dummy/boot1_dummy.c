/**********************************************
 * boot1 : stage1, loaded by stage0 at 0x1000 *
 **********************************************/

#include <sys/reboot.h>
#include <stdio.h>

#define keyboard_waitkeystroke()    getc()

void setup(void)
{
    //heap_init();
}

void main(void)
{
    setup();

    // detach printing of stage 1 from stage 0
    puts("\r\n");

    printf("Welcome in bootloader %s v%u.%u, stage1\r\n",
            "dummy",
            0,
            0);

    puts("Press any key to reboot");
    keyboard_waitkeystroke();

    // make space between the different boots
    puts("\r\n\r\n");

    //reboot();
}

