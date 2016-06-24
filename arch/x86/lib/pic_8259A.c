#include "arch/x86/pic.h"

#include <arch/x86/cpu.h>
#include <arch/x86/ioport.h>
#include <assert.h>

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21

#define PIC2_COMMAND  0xA0
#define PIC2_DATA     0xA1

#define PIC1_ICW1 PIC1_COMMAND
#define PIC1_ICW2 PIC1_DATA
#define PIC1_ICW3 PIC1_DATA
#define PIC1_ICW4 PIC1_DATA

#define PIC2_ICW1 PIC2_COMMAND
#define PIC2_ICW2 PIC2_DATA
#define PIC2_ICW3 PIC2_DATA
#define PIC2_ICW4 PIC2_DATA

#define PIC_ICW1_DEFAULT      0x12
#define PIC_ICW1_LEVEL_DETECT (1 << 3)
#define PIC_ICW1_EDGE_DETECT  (~(1 << 3))
#define PIC_ICW1_WITH_ICW4    (1)
#define PIC_ICW1_WITHOUT_ICW4 (~1)
#define PIC_ICW1_SINGLE       (1 << 1)
#define PIC_ICW1_CASCADED     (~(1 << 1))

#define PIC1_ICW3_SLAVE0 1
#define PIC1_ICW3_SLAVE1 (1 << 1)
#define PIC1_ICW3_SLAVE2 (1 << 2)
#define PIC1_ICW3_SLAVE3 (1 << 3)
#define PIC1_ICW3_SLAVE4 (1 << 4)
#define PIC1_ICW3_SLAVE5 (1 << 5)
#define PIC1_ICW3_SLAVE6 (1 << 6)
#define PIC1_ICW3_SLAVE7 (1 << 7)

#define PIC2_ICW3_CASCADED_ID0 0
#define PIC2_ICW3_CASCADED_ID1 1
#define PIC2_ICW3_CASCADED_ID2 2
#define PIC2_ICW3_CASCADED_ID3 3
#define PIC2_ICW3_CASCADED_ID4 4
#define PIC2_ICW3_CASCADED_ID5 5
#define PIC2_ICW3_CASCADED_ID6 6
#define PIC2_ICW3_CASCADED_ID7 7

#define PIC_ICW4_8086_8088_MODE       1
#define PIC_ICW4_AUTO_EOI             (1 << 1)
#define PIC_ICW4_BUFFERED_MODE1       (1 << 2)
#define PIC_ICW4_BUFFERED_MODE        (1 << 3)
#define PIC_ICW4_NESTED_MODE          (1 << 4)

#define PIC_EOI 0x20 /* End-of-interrupt command code */

void i86_pic_remap(uint8_t pic1_vecoffset, uint8_t pic2_vecoffset, uint16_t mask)
{
    // ICW1
    outb(PIC1_ICW1,
            (PIC_ICW1_DEFAULT & PIC_ICW1_CASCADED & PIC_ICW1_EDGE_DETECT)
            | PIC_ICW1_WITH_ICW4);

    outb(PIC2_ICW1,
            (PIC_ICW1_DEFAULT & PIC_ICW1_CASCADED & PIC_ICW1_EDGE_DETECT)
            | PIC_ICW1_WITH_ICW4);

    // ICW2
    // PIC1 interrupt vector : pic1_vecoffset + 0 --> pic1_vecoffset + 7
    // PIC2 interrupt vector : pic2_vecoffset + 0 --> pic2_vecoffset + 7
    outb(PIC1_ICW2, pic1_vecoffset);
    outb(PIC2_ICW2, pic2_vecoffset);

    // ICW3
    outb(PIC1_ICW3, PIC1_ICW3_SLAVE2);
    outb(PIC2_ICW3, PIC2_ICW3_CASCADED_ID2);

    // ICW4
    outb(PIC1_ICW4, PIC_ICW4_8086_8088_MODE);
    outb(PIC2_ICW4, PIC_ICW4_8086_8088_MODE);

    i86_pic_setintmask(mask);
}

void i86_pic_endofinterrupt(uint8_t intno)
{
    assert(intno < 16);
    outb((intno <= 8) ? PIC1_COMMAND : PIC2_COMMAND, PIC_EOI);
}

uint16_t i86_pic_getintmask(void)
{
    return inb(PIC1_DATA) | (inb(PIC2_DATA) << 8);
}

void i86_pic_setintmask(uint16_t mask)
{
    outb(PIC1_DATA, mask & 0xff);
    outb(PIC2_DATA, mask >> 8);
}
