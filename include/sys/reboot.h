#ifndef _SYS_H
#define _SYS_H

#include "equipment.h"
#include "e820.h"

typedef struct system_info_s
{
    uint8_t          boot_drive_nb;
    equipment_info_t int11_equipments;
    ssize_t          e820_smap_nbentries;
    SMAP_entry_t     e820_smap[20];

} system_info_t;

extern system_info_t system_info;

#endif
