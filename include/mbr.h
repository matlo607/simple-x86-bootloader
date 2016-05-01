#ifndef _MBR_H
#define _MBR_H

#include <stddef.h>
#include <stdint.h>

#define MBR_PARTITION_BOOTABLE_FLAG 0x80

#define MBR_PARTITION_SYSTEMID_EMPTY 0x00
#define MBR_PARTITION_SYSTEMID_FAT12 0x01
#define MBR_PARTITION_SYSTEMID_MINIX14A 0x80
#define MBR_PARTITION_SYSTEMID_MINIX14B 0x81

#define mbr_partition_entry_get_cylinder(sc_pair) \
    ((((sc_pair).cylinder & 0x003) << 8) | ((sc_pair).cylinder >> 2))

typedef struct MBR_partition_entry_s {
    uint8_t boot_flag;
    uint8_t starting_head;
    struct {
        uint16_t sector : 6;
        uint16_t cylinder : 10;
    } starting_SC_pair;
    uint8_t system_id;
    uint8_t ending_head;
    struct {
        uint16_t sector : 6;
        uint16_t cylinder : 10;
    } ending_SC_pair;
    uint32_t LBA;
    uint32_t nb_total_sectors;
} __attribute__((packed)) MBR_partition_entry_t;


#define BOOTSECTOR_SIGNATURE        0xAA55

typedef struct MBR_format_s {
    uint8_t bootstrap[436];
    uint8_t uniq_disk_id[10];
    MBR_partition_entry_t partition_table[4];
    uint16_t signature;
} __attribute__((packed)) MBR_format_t;

MBR_format_t* mbr_get(const char* devname);
void mbr_destroy(MBR_format_t* mbr);

void mbr_print_partition_table(const MBR_partition_entry_t* partition_table);

#endif
