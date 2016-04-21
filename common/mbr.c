#include <mbr.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <arch/x86/cpu.h>
#include <arch/x86/disk.h>
#include <arch/x86/config_mem.h>


#define keyboard_waitkeystroke()    getc()

MBR_format_t* mbr_open(uint8_t drive_nb)
{
    MBR_format_t* mbr = malloc(sizeof(MBR_format_t));
    assert(mbr != NULL);

    uint16_t reg_ds;
    uint16_t offset;
#ifdef BOOTLOADER_PROTECTED_MODE_ENABLED
    reg_ds = (0x000F0000 & (uint32_t)mbr) >> 4;
    offset = (0x0000FFFF & (uint32_t)mbr);
#else
    reg_get_data_segment(&reg_ds);
    offset = (uint16_t)mbr;
#endif

    uint8_t nb_sectors_to_read = 1;
    uint8_t nb_read_sectors = disk_read_sectors(drive_nb, reg_ds, offset, 0, 0, 1, nb_sectors_to_read);
    if (nb_read_sectors != nb_sectors_to_read) {
        printf("disk_read_sectors error : read %u sectors instead of %u\r\n", nb_read_sectors, nb_sectors_to_read);
    }
    printf("bootsector signature = %#04x\r\n", mbr->signature);

    return mbr;
}

MBR_format_t* mbr_read(uint8_t drive_nb)
{
    // TODO
    (void) drive_nb;
    return NULL;
}

size_t mbr_write(uint8_t drive_nb, MBR_format_t* mbr)
{
    // TODO
    (void) drive_nb;
    (void) mbr;
    return 0;
}

void mbr_close(MBR_format_t* mbr)
{
    free(mbr);
}

void mbr_print_partition_table(const MBR_partition_entry_t* partition_table)
{
    for (uint8_t i = 0; i < 4; ++i) {
        const MBR_partition_entry_t* entry = &partition_table[i];

        printf("\t[partition %u]\r\n", i);

        printf("\tboot flag: %s\r\n", (entry->boot_flag == MBR_PARTITION_BOOTABLE_FLAG) ? "yes":"no");

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
        keyboard_waitkeystroke();
    }
}
