#include "arch/x86/disk.h"

#include <stdio.h>
#include <arch/x86/bios.h>

bool disk_reset(uint8_t drive_nb)
{
    bool error = false;

    //__asm__ __volatile__ (
    //        "movb %[bios_service_reset_disk_ctrl], %%ah;"
    //        "movb %[drive_nb], %%dl;"
    //        "int $0x13;"
    //        "jc 0f;"
    //        "movb $1, %[res];"
    //        "0:;"
    //        : [res] "=g" (succeeded)
    //        : [bios_service_reset_disk_ctrl] "i" (0x00),
    //          [drive_nb] "g" (drive_nb)
    //        : "%eax", "%edx"
    //        );

    x86_regs_t regs_in, regs_out;

    x86_regs_init(&regs_in);

    regs_in.D._Rl = drive_nb;
    regs_in.A._Rh = 0x00;

    bioscall(0x13, &regs_in, &regs_out);

    error = regs_out.eflags.CF;

    return !error;
}

uint8_t disk_read_sectors(uint8_t drive_nb,
                          uint16_t segment_base,
                          uint16_t offset,
                          uint16_t head,
                          uint16_t cylinder,
                          uint8_t sector,
                          uint8_t nb_sectors)
{
    uint16_t start_cylinder_sector = ((cylinder & 0x00FF) << 8) | ((cylinder & 0x0300) >> 2) | sector;

    //__asm__ __volatile__ (
    //        "push %%es;"
    //        "movb %[bios_service_read_sector_disk], %%ah;"
    //        "movb %[nb_sectors], %%al;"
    //        "movb %[drive_nb], %%dl;"
    //        "movw %[segment_addr], %%es;"
    //        "movw %[offset], %%bx;"
    //        "movb %[head], %%dh;"
    //        "movw %[cylinder_head_regs], %%cx;"
    //        "int $0x13;"
    //        "movw %%ax, %[op_status];"
    //        "pop %%es;"
    //        : [op_status] "=g" (status)
    //        : [bios_service_read_sector_disk] "i" (0x02),
    //          [nb_sectors] "g" (nb_sectors),
    //          [drive_nb] "g" (drive_nb),
    //          [segment_addr] "g" (segment_addr),
    //          [offset] "g" (offset),
    //          [head] "g" (head),
    //          [cylinder_head_regs] "g" (cylinder_head_regs)
    //        : "%eax", "%ebx", "%ecx", "%edx", "cc"
    //        );

    x86_regs_t regs_in, regs_out;

    x86_regs_init(&regs_in);

    regs_in.A._Rl = nb_sectors;
    regs_in.D._Rl = head;
    regs_in.C._Rx = start_cylinder_sector;
    regs_in.D._Rl = drive_nb;
    regs_in.B._Rx = offset;
    regs_in.ES = segment_base;
    regs_in.A._Rh = 0x02;

    bioscall(0x13, &regs_in, &regs_out);

    //bool error = regs_out.eflags.CF;
    uint8_t effectively_read_sectors = regs_out.A._Rl;

    return effectively_read_sectors;
}
