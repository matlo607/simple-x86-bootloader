#include <mbr.h>

#include <dev.h>
#include <block_dev.h>

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <arch/x86/cpu.h>
#include <arch/x86/disk.h>
#include <arch/x86/config_mem.h>


#define keyboard_waitkeystroke()    getc()

MBR_format_t* mbr_get(const char* devname)
{
    dev_t* device = dev_lookuptable(devname);
    if (!device) {
        errno = ENODEV;
        return NULL;
    }

    block_drv_op_t* op = block_dev_driver_lookuptable(device->major);
    assert(op != NULL);

    MBR_format_t* mbr = malloc(sizeof(MBR_format_t));
    assert(mbr != NULL);

    if (op->block_drv_open(device->minor) == EXIT_FAILURE) {
        errno = ENODEV;
        free(mbr);
        return NULL;
    }

    keyboard_waitkeystroke();

    // Read the first sector of the device
    ssize_t read = op->block_drv_read(device->minor, 0, 1, mbr);
    if (read < 0 || read < 512) {
        free(mbr);
        return NULL;
    }

    printf("bootsector signature = %#04x\r\n", mbr->signature);

    return mbr;
}

void mbr_destroy(MBR_format_t* mbr)
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
