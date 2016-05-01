/**********************************************
 * boot1 : stage1, loaded by stage0 at 0x1000 *
 **********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <shell.h>
#include <sys/system.h>
#include <sys/setup.h>
#include <version.h>

// to remove from here and create an interface to them because
// this is specific to i386
#include <arch/x86/graphics.h>
#include <arch/x86/video.h>
#include <arch/x86/disk.h>
#include <arch/x86/cpu.h>
#include <arch/x86/bios.h>
#include <multiboot.h>

#include <sys/vga.h>
#include <sys/tty.h>

#define keyboard_waitkeystroke()    getc()

void setup(void)
{
#ifndef BOOTLOADER_PROTECTED_MODE_ENABLED
    platform_specific_startup();
#endif

    platform_startup();

    shell_init();
}

void main(void)
{
#ifdef BOOTLOADER_PROTECTED_MODE_ENABLED
    //x86_regs_t regs_in, regs_out;

    //x86_regs_init(&regs_in);

    //regs_in.A._Rl = 'M';
    //regs_in.B._Rl = 0x07;
    //regs_in.A._Rh = 0x0e;

    //real_mode_int(0x10, &regs_in, &regs_out);
#else

#endif

    setup();

    printf("Welcome in bootloader %s release v%u.%u, stage1\r\n",
            BOOTLOADER_RELEASE_MODE,
            BOOTLOADER_VERSION,
            BOOTLOADER_REVISION);

    multiboot_fill_header();

    keyboard_waitkeystroke();

    // change to graphic mode
    video_setmode(VIDEO_MODE_GRAPHICS_256C_640x480);

    graphics_draw_rectangle(5, 5, 100, 100, 0x2, true);
    graphics_draw_rectangle(10, 10, 90, 90, 0x3, false);

    graphics_draw_line(5, 7, 100, 90, 0x1);
    graphics_draw_line(0, 0, 100, 90, 0x1);
    graphics_draw_line(0, 100, 90, 90, 0x1);
    graphics_draw_line(50, 0, 50, 100, 0x1);
    graphics_draw_line(0, 50, 100, 50, 0x1);
    graphics_draw_line(100, 5, 8, 90, 0x1);

    keyboard_waitkeystroke();

    // change back to text mode
    video_setmode(VIDEO_MODE_TEXT_16C_80x25_640x200);

    while (1) {
        shell_main();
    }

    puts("Press any key to reboot");
    keyboard_waitkeystroke();
    reboot();

    //puts("Wait forever ...");
    //hang_forever();
}
