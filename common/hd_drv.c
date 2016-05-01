#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include <debug.h>
#include <block_dev.h>

#include <arch/x86/config_mem.h>
#include <arch/x86/disk.h>
#include <arch/x86/cpu.h>

#define BIOS_HD_DRIVE_BASE_NB 0x80

#define hd_getstatus(minor) \
    dev_getstatus(HARD_DISK_DRIVER, minor);

#define hd_setstatus(minor, status) \
    dev_setstatus(HARD_DISK_DRIVER, minor, status);

#define hd_getinfo(minor) \
    dev_getinfo(HARD_DISK_DRIVER, minor);

#define hd_setinfo(minor, info) \
    dev_setinfo(HARD_DISK_DRIVER, minor, info);


int hd_open(minor_t minor)
{
    devstatus_t dev_status = hd_getstatus(minor);
    if (dev_status == DEV_STATUS_NOT_FOUND) {
        errno = ENODEV;
        return EXIT_FAILURE;
    }
#ifdef DEBUG_HD_DRIVER 
    debug_printf("hd(%u,%u): status = %d\r\n", HARD_DISK_DRIVER, minor, dev_status);
#endif

    // reset disk
    if (dev_status == DEV_STATUS_UNKNOWN) {
#ifdef DEBUG_HD_DRIVER 
        debug_printf("hd(%u,%u): attempt to reset disk ", HARD_DISK_DRIVER, minor, dev_status);
#endif

        if (disk_reset(BIOS_HD_DRIVE_BASE_NB + minor) == EXIT_FAILURE) {
#ifdef DEBUG_HD_DRIVER 
            debug_printf("[NOK]\r\n");
#endif
            errno = ENODEV;
            return EXIT_FAILURE;
        }
        hd_setstatus(minor, DEV_STATUS_INITIALIZED);
#ifdef DEBUG_HD_DRIVER 
        debug_printf("[OK]\r\n");
#endif
    }

    // set info
    chs_disk_geometry_t* dev_info = (chs_disk_geometry_t*) hd_getinfo(minor);
    if (dev_info == NULL) {
#ifdef DEBUG_HD_DRIVER 
        debug_printf("hd(%u,%u): get disk geometry ", HARD_DISK_DRIVER, minor, dev_status);
#endif

        chs_disk_geometry_t* geometry = malloc(sizeof(chs_disk_geometry_t));
        assert(geometry != NULL);
        if (disk_geometry(BIOS_HD_DRIVE_BASE_NB + minor, geometry) == EXIT_FAILURE) {
#ifdef DEBUG_HD_DRIVER 
            debug_printf("[NOK]\r\n");
#endif
            free(geometry);
        } else {
#ifdef DEBUG_HD_DRIVER 
            debug_printf("[OK]\r\n");
#endif
            chs_disk_geometry_print(geometry);
            hd_setinfo(minor, geometry);
        }
    }

    return EXIT_SUCCESS;
}

ssize_t hd_read(minor_t minor, lbaint_t lba_start, lbaint_t count, void* buffer)
{
    ssize_t read = 0;
    CHS_t chs_addr_start;

    chs_disk_geometry_t* dev_info = (chs_disk_geometry_t*) hd_getinfo(minor);
    assert(dev_info != NULL);
    // check that the number of sectors per track is not greated than what I can write in a real mode segment
    assert(dev_info->NS <= (SEGMENT_SIZE_16bit_MODE / SECTOR_SIZE));
    // check parameters
    assert(lba_start < dev_info->Nlba);
    assert(lba_start + count <= dev_info->Nlba);

    uint16_t buf_rm_segment;
    uint16_t buf_rm_offset;
#ifdef BOOTLOADER_PROTECTED_MODE_ENABLED
    buf_rm_segment = SEGMENT_EXTRA_DATA_RM;
    buf_rm_offset = 0x0000;
#else
    reg_get_data_segment(&buf_rm_segment);
    buf_rm_offset = (uint16_t)(uint32_t)buffer;
#endif

    uint8_t nb_sectors_to_read = count;
    while (count > 0) {
        // convert LBA address to CHS address
        lbatochs(dev_info, lba_start, &chs_addr_start);
#ifdef DEBUG_HD_DRIVER 
        debug_printf("hd(%u,%u): req READ (lba_start = %u <=> CHS=(%u,%u,%u), count = %u)\r\n",
                HARD_DISK_DRIVER, minor, lba_start,
                chs_addr_start.C, chs_addr_start.H, chs_addr_start.S, count);
#endif


        // check track bounds
        if (count > dev_info->NS) {
            nb_sectors_to_read = dev_info->NS - chs_addr_start.S + 1;
#ifdef DEBUG_HD_DRIVER 
            debug_printf("hd(%u,%u): resize req to (lba_start = %u, count = %u)\r\n",
                    HARD_DISK_DRIVER, minor, lba_start, nb_sectors_to_read);
#endif
        }

#ifdef DEBUG_HD_DRIVER 
        debug_printf("hd(%u,%u): [bios] disk %#02x, attempt to read %u sectors",
                HARD_DISK_DRIVER, minor, BIOS_HD_DRIVE_BASE_NB + minor, nb_sectors_to_read);
#endif
        uint8_t nb_read_sectors = 0;
        for (uint8_t i=0; i<5 && (nb_read_sectors != nb_sectors_to_read); i++) {
            nb_read_sectors = disk_read_sectors(BIOS_HD_DRIVE_BASE_NB + minor, buf_rm_segment, buf_rm_offset, &chs_addr_start, nb_sectors_to_read);
        }

#ifdef DEBUG_HD_DRIVER 
        if (nb_read_sectors != nb_sectors_to_read) {
            debug_printf(", read %u sectors [NOK]\r\n", nb_read_sectors);
        } else {
            debug_printf(" [OK]\r\n");
        }
#endif

        size_t nb_read_bytes = nb_read_sectors * SECTOR_SIZE;

#ifdef BOOTLOADER_PROTECTED_MODE_ENABLED
        // copy data to buffer
        memcpy(buffer, (void*)(buf_rm_segment << 4 | buf_rm_offset), nb_read_bytes);
        buf_rm_offset = 0x0000;
#else
        buf_rm_offset += nb_read_bytes;
#endif

        read += nb_read_bytes;
        count -= nb_read_sectors;
        lba_start += nb_read_sectors;
    }

    return read;
}

ssize_t hd_write(minor_t minor, lbaint_t lba_start, lbaint_t count, void* buffer)
{
    (void) minor;
    (void) lba_start;
    (void) count;
    (void) buffer;

    ssize_t write = 0;

    return write;
}

int hd_close(minor_t minor)
{
    (void) minor;

    return EXIT_SUCCESS;
}
