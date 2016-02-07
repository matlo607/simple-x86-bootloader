#include "disk.h"

bool disk_reset(uint8_t drive_nb)
{
    bool succeeded = false;

    __asm__ __volatile__ (
            "movb %[bios_service_reset_disk_ctrl], %%ah;"
            "movb %[drive_nb], %%dl;"
            "int $0x13;"
            "jc 0f;"
            "movb $1, %[res];"
            "0:;"
            : [res] "=g" (succeeded)
            : [bios_service_reset_disk_ctrl] "i" (0x00),
              [drive_nb] "g" (drive_nb)
            : "%eax", "%edx"
            );

    return succeeded;
}

uint8_t disk_read_sectors(uint8_t drive_nb,
                          uint16_t segment_addr,
                          uint16_t offset,
                          uint8_t head,
                          uint16_t cylinder,
                          uint8_t sector)
{
    uint8_t nb_read_sectors = 0;

    uint16_t cylinder_head_regs = (cylinder << 8) | ((cylinder & 0x0300) >> 2) | sector;

    __asm__ __volatile__ (
            "push %%edx;"
            "push %%es;"
            "movb %[bios_service_read_sector_disk], %%ah;"
            "movb %[drive_nb], %%dl;"
            "movw %[segment_addr], %%es;"
            "movw %[offset], %%bx;"
            "movb %[head], %%dh;"
            "movw %[cylinder_head_regs], %%cx;"
            "int $0x13;"
            "addb %%al, %[nb_read_sectors];"
            "pop %%es;"
            "pop %%edx;"
            : [nb_read_sectors] "=g" (nb_read_sectors)
            : [bios_service_read_sector_disk] "i" (0x02),
              [drive_nb] "g" (drive_nb),
              [segment_addr] "g" (segment_addr),
              [offset] "g" (offset),
              [head] "g" (head),
              [cylinder_head_regs] "g" (cylinder_head_regs)
            : "%eax", "%ebx", "%ecx", "cc"
            );

    return nb_read_sectors;
}

