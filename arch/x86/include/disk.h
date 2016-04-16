#ifndef _DISK
#define _DISK

#include <stdint.h>
#include <stdbool.h>

extern const uint8_t boot_drive_nb;

bool disk_reset(uint8_t drive_nb);

uint8_t disk_read_sectors(uint8_t drive_nb,
                          uint16_t segment_addr,
                          uint16_t offset,
                          uint16_t head,
                          uint16_t cylinder,
                          uint8_t sector,
                          uint8_t nb_sectors);

#endif
