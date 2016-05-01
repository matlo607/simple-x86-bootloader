#include "block_dev.h"

#include <assert.h>
#include <stdio.h>

#define BLOCK_DEV_DRIVERS_NB 2
static block_dev_driver_t block_dev_drivers[BLOCK_DEV_DRIVERS_NB] =
{
    {
        .major = HARD_DISK_DRIVER,
        .ops = {
            &hd_open,
            &hd_read,
            &hd_write,
            &hd_close
        },
    },
    {
        .major = FLOPPY_DISK_DRIVER,
        .ops = {
            NULL,
            NULL,
            NULL,
            NULL
        },
    }
};

void lbatochs(const chs_disk_geometry_t* desc, lbaint_t lba_addr, CHS_t* chs_addr)
{
    assert(desc != NULL);
    assert(chs_addr != NULL);

    chs_addr->S = (lba_addr % desc->NS) + 1;
    chs_addr->H = ( (lba_addr - (chs_addr->S - 1)) / desc->NS ) % desc->NH;
    chs_addr->C = (lba_addr - (chs_addr->S - 1)) / desc->NS / desc->NH;
}

lbaint_t chstolba(const chs_disk_geometry_t* desc, const CHS_t* chs_addr)
{
    assert(desc != NULL);
    assert(chs_addr != NULL);

    return (chs_addr->C * desc->NS * desc->NH) + (chs_addr->H * desc->NS) + (chs_addr->S - 1);
}

void chs_disk_geometry_print(const chs_disk_geometry_t* geometry)
{
    assert(geometry);

    printf("\tNLBA = %u | (NC, NH, NS) = (%u, %u, %u)\r\n",
            geometry->Nlba, geometry->NC, geometry->NH, geometry->NS);
}

block_drv_op_t* block_dev_driver_lookuptable(major_t major)
{
    for (uint8_t driver_index = 0; driver_index < BLOCK_DEV_DRIVERS_NB; driver_index++) {
        if (major == block_dev_drivers[driver_index].major) {
            return &block_dev_drivers[driver_index].ops;
        }
    }

    return NULL;
}
