#include "arch/x86/disk.h"

#include <stdio.h>
#include <arch/x86/bios.h>

bool disk_reset(uint8_t drive_nb)
{
    bool error = false;

#ifdef BOOTLOADER_PROTECTED_MODE_ENABLED
    regs16_t regs;

    regs.d._Rl = drive_nb;
    regs.a._Rh = 0x00;

    int32(0x13, &regs);

    error = regs.eflags.CF;
#else
    x86_regs_t regs_in, regs_out;

    x86_regs_init(&regs_in);

    regs_in.D._Rl = drive_nb;
    regs_in.A._Rh = 0x00;

    bioscall(0x13, &regs_in, &regs_out);

    error = regs_out.eflags.CF;
#endif

    return !error;
}

uint8_t disk_read_sectors(
        uint8_t drive_nb,
        uint16_t segment_base,
        uint16_t offset,
        uint16_t head,
        uint16_t cylinder,
        uint8_t sector,
        uint8_t nb_sectors)
{
    uint16_t start_cylinder_sector = ((cylinder & 0x00FF) << 8) | ((cylinder & 0x0300) >> 2) | sector;
    uint8_t effectively_read_sectors = 0;

#ifdef BOOTLOADER_PROTECTED_MODE_ENABLED
    regs16_t regs;

    regs.a._Rl = nb_sectors;
    regs.d._Rh = head;
    regs.c._Rx = start_cylinder_sector;
    regs.d._Rl = drive_nb;
    regs.b._Rx = offset;
    regs.es = segment_base;
    regs.a._Rh = 0x02;

    int32(0x13, &regs);

    effectively_read_sectors = regs.a._Rl;
#else
    x86_regs_t regs_in, regs_out;

    x86_regs_init(&regs_in);

    regs_in.A._Rl = nb_sectors;
    regs_in.D._Rh = head;
    regs_in.C._Rx = start_cylinder_sector;
    regs_in.D._Rl = drive_nb;
    regs_in.B._Rx = offset;
    regs_in.ES = segment_base;
    regs_in.A._Rh = 0x02;

    bioscall(0x13, &regs_in, &regs_out);

    //bool error = regs_out.eflags.CF;
    effectively_read_sectors = regs_out.A._Rl;
#endif

    return effectively_read_sectors;
}
