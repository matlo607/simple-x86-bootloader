#include "multiboot.h"

#include <stdio.h>
#include <stdlib.h>

#include <arch/x86/disk.h>
#include <mbr.h>
#include <arch/x86/int12.h>
#include <arch/x86/e801.h>
#include <arch/x86/e820.h>

multiboot_info_t multiboot_header;


void multiboot_fill_header(void)
{
    /********************************
     * multiboot_info.boot_device : *
     ********************************/
    {
        multiboot_header.drive = boot_drive_nb;
        printf("Boot device: %s (%#02x)\r\n",
                (boot_drive_nb & 0x80) ? "disk" : "floppy",
                boot_drive_nb);

        multiboot_header.part1 = MULTIBOOT_INFO_UNUSED_PARTITION;
        multiboot_header.part2 = MULTIBOOT_INFO_UNUSED_PARTITION;
        multiboot_header.part3 = MULTIBOOT_INFO_UNUSED_PARTITION;

        MBR_format_t* mbr = mbr_get("hda");
        if (mbr != NULL) {

            for (uint8_t i=0; i<4; ++i) {
                if (mbr->partition_table[i].boot_flag == MBR_PARTITION_BOOTABLE_FLAG) {
                    printf("MBR: boot from partition %u\r\n", i);
                    multiboot_header.part1 = i;
                    break;
                }
            }
            mbr_print_partition_table((const MBR_partition_entry_t*)&(mbr->partition_table));
            mbr_destroy(mbr);

        } else {
            printf("MBR: error reading the boot sector\r\n");
        }
    }

    /******************************
     * multiboot_info.mem_lower : *
     ******************************/
    {
        ssize_t lower_memory = 0;
        if ((lower_memory = int12_detect_lower_mem()) < 0) {
            puts("int 0x12 : fail to get the size of lower memory\r\n");
        } else {
            multiboot_header.mem_lower = (size_t)lower_memory;
            printf("Lower memory: %dKB\r\n", lower_memory);
        }
    }

    /******************************
     * multiboot_info.mem_upper : *
     ******************************/
    {
        e801_upper_mem_t upper_memory;

        if (e801_detect_upper_mem(&upper_memory)) {
            printf("Upper memory: %uKB [0x%x] (maximum 15MB [0x3c00])\r\n", upper_memory._1MB_to_16MB, upper_memory._1MB_to_16MB);
            printf("              %uKB [0x%x] (%uMB)\r\n", upper_memory._16MB_to_4GB, upper_memory._16MB_to_4GB, upper_memory._16MB_to_4GB / 1024);

            // only report the first memory hole
            multiboot_header.mem_upper = upper_memory._1MB_to_16MB;
        }
    }

    /***************************
     * multiboot_info.mmap_* : *
     ***************************/
    {
        SMAP_entry_t e820_smap[20];
        ssize_t e820_smap_nb_entries = e820_detect_mem(e820_smap, 20);

        multiboot_header.mmap_addr = (uint32_t)NULL;
        multiboot_header.mmap_length = 0;

        if (e820_smap_nb_entries == -1) {
            printf("int 0x15, 0xe820 : error while mapping memory\r\n");

        } else {
            multiboot_memory_map_t* entries = malloc(e820_smap_nb_entries * sizeof(multiboot_memory_map_t));
            if (entries != NULL) {
                for (ssize_t i=0; i<e820_smap_nb_entries; i++) {
                    entries[i].size = 20;
                    entries[i].addr = e820_smap[i].baseL;
                    entries[i].len = e820_smap[i].lengthL;
                    entries[i].type = e820_smap[i].type;
                }
            }

            multiboot_header.mmap_length = e820_smap_nb_entries;
            multiboot_header.mmap_addr = (uint32_t)entries;

            printf("Memory map (nb entries = %d)\r\n", e820_smap_nb_entries);
            e820_display_SMAP_entries(e820_smap, e820_smap_nb_entries);
        }
    }
}
