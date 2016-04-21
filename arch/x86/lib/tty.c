#include <sys/tty.h>

#include <arch/x86/bios.h>
#include <sys/vga.h>

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

void tty_initialize(void)
{
#ifdef BOOTLOADER_PROTECTED_MODE_ENABLED

#ifdef PM_DRIVERS
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = make_color(COLOR_LIGHT_GREEN, COLOR_BLACK);
	terminal_buffer = VGA_MEMORY;
	for ( size_t y = 0; y < VGA_HEIGHT; y++ )
	{
		for ( size_t x = 0; x < VGA_WIDTH; x++ )
		{
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = make_vgaentry(' ', terminal_color);
		}
	}
#endif

#endif
}

void tty_set_color(uint8_t color)
{
	terminal_color = color;
}

void tty_put_entry_at(char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = make_vgaentry(c, color);
}

void tty_put_char(char c)
{
#ifdef BOOTLOADER_PROTECTED_MODE_ENABLED

#ifdef PM_DRIVERS
    if (c == '\r') {
        terminal_column = 0;

    } else if (c == '\n') {
        if ( ++terminal_row == VGA_HEIGHT ) {
            terminal_row = 0;
        }

    } else if (c == '\t') {
        tty_put_char(' ');
        tty_put_char(' ');
        tty_put_char(' ');
        tty_put_char(' ');

    } else {
        tty_put_entry_at(c, terminal_color, terminal_column, terminal_row);

        if ( ++terminal_column == VGA_WIDTH ) {
            terminal_column = 0;
            if ( ++terminal_row == VGA_HEIGHT )
            {
                terminal_row = 0;
            }
        }
    }
#else
    regs16_t regs;

    regs.a._Rl = c;
    regs.b._Rl = 0x07;
    regs.a._Rh = 0x0e;

    int32(0x10, &regs);
#endif

#else
    x86_regs_t regs_in, regs_out;

    x86_regs_init(&regs_in);

    regs_in.A._Rl = c;
    regs_in.B._Rl = 0x07;
    regs_in.A._Rh = 0x0e;

    bioscall(0x10, &regs_in, &regs_out);
#endif
}
