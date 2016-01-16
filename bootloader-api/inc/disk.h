#ifndef _DISK
#define _DISK

#include "common.h"

extern const uint8_t boot_drive_nb;

extern bool disk_reset(uint8_t drive_nb);

extern uint8_t disk_read_sectors(uint8_t drive_nb,
                                 uint16_t segment_addr,
                                 uint16_t offset,
                                 uint8_t head,
                                 uint16_t cylinder,
                                 uint8_t sector);

#endif
