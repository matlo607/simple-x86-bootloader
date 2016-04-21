#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MBR_PARTITION_BOOTABLE_FLAG_SET   0x80
#define MBR_PARTITION_BOOTABLE_FLAG_UNSET 0x00

#define MBR_PARTITION_SYSTEMID_EMPTY 0x00
#define MBR_PARTITION_SYSTEMID_FAT12 0x01
#define MBR_PARTITION_SYSTEMID_MINIX14A 0x80
#define MBR_PARTITION_SYSTEMID_MINIX14B 0x81

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

#define mbr_partition_entry_get_cylinder(sc_pair) \
    ((((sc_pair).cylinder & 0x003) << 8) | ((sc_pair).cylinder >> 2))

#define mbr_partition_entry_set_cylinder(sc_pair, C) \
    (sc_pair).cylinder = (((C) & 0x0FF) << 2) | (((C) & 0x3FF) >> 8)


#define BOOTSECTOR_SIGNATURE        0xAA55

typedef struct MBR_format_s {
    uint8_t bootstrap[436];
    uint8_t uniq_disk_id[10];
    MBR_partition_entry_t partition_table[4];
    uint16_t signature;
} __attribute__((packed)) MBR_format_t;


void mbr_part_tab_print(const MBR_partition_entry_t* partition_table);
void part(MBR_partition_entry_t* partition_table);

int main(int argc, char* argv[])
{
    (void) argc;
    char* bootsector_filename = argv[1];
    MBR_format_t mbr_bootsector;
    FILE* bootsector = NULL;

    bootsector = fopen(bootsector_filename, "rb");
    if (bootsector == NULL) {
        perror(strerror(errno));
        return EXIT_FAILURE;
    }

    size_t bytes_read = fread(&mbr_bootsector, sizeof(MBR_format_t), 1, bootsector);
    if (bytes_read < 1) {
        if (feof(bootsector)) {
            perror("Length of the provided bootsector is less than 512 bytes");
        } else if (ferror(bootsector)) {
            perror("unkown error with the provided file");
        }
        return EXIT_FAILURE;
    }

    fclose(bootsector);

    mbr_part_tab_print((const MBR_partition_entry_t*)&mbr_bootsector.partition_table);

    part((MBR_partition_entry_t*)&mbr_bootsector.partition_table);

    mbr_part_tab_print((const MBR_partition_entry_t*)&mbr_bootsector.partition_table);

    bootsector = fopen(bootsector_filename, "wb");

    size_t bytes_written = fwrite(&mbr_bootsector, sizeof(MBR_format_t), 1, bootsector);
    if (bytes_written < 1) {
        if (feof(bootsector)) {
            perror("Length of the provided bootsector is less than 512 bytes");
        } else if (ferror(bootsector)) {
            perror("unkown error with the provided file");
        }
        return EXIT_FAILURE;
    }

    fclose(bootsector);

    return EXIT_SUCCESS;
}

void mbr_part_tab_print(const MBR_partition_entry_t partition_table[])
{
    for (uint8_t i = 0; i < 4; ++i) {
        const MBR_partition_entry_t* entry = &partition_table[i];

        printf("\t[partition %u]\r\n", i);

        printf("\tboot flag: %s\r\n", (entry->boot_flag == MBR_PARTITION_BOOTABLE_FLAG_SET) ? "yes":"no");

        const char* type_str;
        switch (entry->system_id) {
            case MBR_PARTITION_SYSTEMID_EMPTY:
                type_str = "empty partition";
                break;

            case MBR_PARTITION_SYSTEMID_FAT12:
                type_str = "FAT12";
                break;

            case MBR_PARTITION_SYSTEMID_MINIX14A:
                type_str = "Minix 1.4a";
                break;

            case MBR_PARTITION_SYSTEMID_MINIX14B:
                type_str = "Minix 1.4b";
                break;

            default:
                type_str = "unknown";
                break;
        }
        printf("\ttype : %s\r\n", type_str);

        printf("\tbegin: (C,H,S) = (%u, %u, %u)\r\n",
                entry->starting_head,
                mbr_partition_entry_get_cylinder(entry->starting_SC_pair),
                entry->starting_SC_pair.sector);

        printf("\tend: (C,H,S) = (%u, %u, %u)\r\n",
                entry->ending_head,
                mbr_partition_entry_get_cylinder(entry->ending_SC_pair),
                entry->ending_SC_pair.sector);

        printf("\tLBA: %u\r\n", entry->LBA);

        printf("\tTotal (in sectors): %u\r\n", entry->nb_total_sectors);

        puts("\r\n");
    }
}

void part(MBR_partition_entry_t* partition_table)
{
    MBR_partition_entry_t* entry = &partition_table[0];
    {
        entry->boot_flag = MBR_PARTITION_BOOTABLE_FLAG_SET;

        entry->starting_head = 0;
        entry->starting_SC_pair.sector = 37;
        mbr_partition_entry_set_cylinder(entry->starting_SC_pair, 0);

        entry->system_id = MBR_PARTITION_SYSTEMID_MINIX14A;

        entry->ending_head = 0;
        entry->ending_SC_pair.sector = 50;
        mbr_partition_entry_set_cylinder(entry->ending_SC_pair, 0);

        entry->LBA = 36;
        entry->nb_total_sectors = 50 - 37 + 1;
    }

    entry = &partition_table[1];
    {
        entry->boot_flag = MBR_PARTITION_BOOTABLE_FLAG_UNSET;

        entry->starting_head = 0;
        entry->starting_SC_pair.sector = 51;
        mbr_partition_entry_set_cylinder(entry->starting_SC_pair, 0);

        entry->system_id = MBR_PARTITION_SYSTEMID_FAT12;

        entry->ending_head = 0;
        entry->ending_SC_pair.sector = 62;
        mbr_partition_entry_set_cylinder(entry->ending_SC_pair, 0);

        entry->LBA = 50;
        entry->nb_total_sectors = 62 - 51 + 1;
    }
}
