#ifndef _DISK
#define _DISK

#include <arch/x86/checkCCversion.h>

#include <stdint.h>
#include <stdbool.h>

#include <block_dev.h>

extern const uint8_t boot_drive_nb;

int disk_reset(uint8_t drive_nb);

int disk_geometry(uint8_t drive_nb, chs_disk_geometry_t* geometry);

uint8_t disk_read_sectors(uint8_t drive_nb,
                          uint16_t segment_base,
                          uint16_t offset,
                          CHS_t* chs_addr,
                          uint8_t nb_sectors);

#endif
