#include "arch/x86/disk.h"

#include <stdio.h>
#include <assert.h>
#include <arch/x86/bios.h>

int disk_reset(uint8_t drive_nb)
{
    int status = EXIT_SUCCESS;

#ifdef BOOTLOADER_PROTECTED_MODE_ENABLED
    regs16_t regs;

    regs.d._Rl = drive_nb;
    regs.a._Rh = 0x00;

    int32(0x13, &regs);

    if (regs.eflags.CF) {
        status = EXIT_FAILURE;
    }
#else
    x86_regs_t regs_in, regs_out;

    x86_regs_init(&regs_in);

    regs_in.D._Rl = drive_nb;
    regs_in.A._Rh = 0x00;

    bioscall(0x13, &regs_in, &regs_out);

    if (regs_out.eflags.CF) {
        status = EXIT_FAILURE;
    }
#endif

    return status;
}

int disk_geometry(uint8_t drive_nb, chs_disk_geometry_t* geometry)
{
    assert(geometry != NULL);

#ifdef BOOTLOADER_PROTECTED_MODE_ENABLED
    regs16_t regs;

    regs.d._Rl = drive_nb;
    regs.a._Rh = 0x08;

    int32(0x13, &regs);

    if (regs.eflags.CF && regs.a._Rh == 0x01)
    {
        printf("disk geometry error : invalid command\r\n");
        return EXIT_FAILURE;
    }

    //uint8_t floppy_type = regs.b._Rl;
    //debug_printf("floppy type = %u\r\n", floppy_type);

    geometry->NC = (regs.c._Rh | ((regs.c._Rl & 0xc0) << 2)) + 1;
    geometry->NS = regs.c._Rl & ~0xc0;
    geometry->NH = regs.d._Rh + 1;

    //uint8_t nb_drives = regs.d._Rl;
    //debug_printf("number of drives = %u\r\n", nb_drives);
#else
    x86_regs_t regs_in, regs_out;

    x86_regs_init(&regs_in);

    regs_in.D._Rl = drive_nb;
    regs_in.A._Rh = 0x08;

    bioscall(0x13, &regs_in, &regs_out);

    if (regs_out.eflags.CF && regs_out.A._Rh == 0x01)
    {
        printf("disk geometry error : invalid command\r\n");
        return EXIT_FAILURE;
    }

    //uint8_t floppy_type = regs_out.B._Rl;
    //debug_printf("floppy type = %u\r\n", floppy_type);

    geometry->NC = (regs_out.C._Rh | ((regs_out.C._Rl & 0xc0) << 2)) + 1;
    geometry->NS = regs_out.C._Rl & ~0xc0;
    geometry->NH = regs_out.D._Rh + 1;

    //uint8_t nb_drives = regs_out.D._Rl;
    //debug_printf("number of drives = %u\r\n", nb_drives);
#endif

    geometry->Nlba = geometry->NC * geometry->NH * geometry->NS;

    return EXIT_SUCCESS;
}

uint8_t disk_read_sectors(
        uint8_t drive_nb,
        uint16_t segment_base,
        uint16_t offset,
        CHS_t* chs_addr,
        uint8_t nb_sectors)
{
    uint16_t start_cylinder_sector = ((chs_addr->C & 0x00FF) << 8) | ((chs_addr->C & 0x0300) >> 2) | chs_addr->S;
    uint8_t effectively_read_sectors = 0;

#ifdef BOOTLOADER_PROTECTED_MODE_ENABLED
    regs16_t regs;

    regs.a._Rl = nb_sectors;
    regs.d._Rh = chs_addr->H;
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
    regs_in.D._Rh = chs_addr->H;
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
