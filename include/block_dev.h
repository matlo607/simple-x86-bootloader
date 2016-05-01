#ifndef _BLOCK_DEV_H
#define _BLOCK_DEV_H

#include <stddef.h>
#include <errno.h>
#include <dev.h>

typedef uint32_t lbaint_t;

typedef struct {
    uint16_t C;
    uint8_t H;
    uint8_t S;
} CHS_t;

typedef struct {
    uint16_t NC;
    uint8_t NH;
    uint8_t NS;
    lbaint_t Nlba;
} chs_disk_geometry_t;

void lbatochs(const chs_disk_geometry_t* desc, lbaint_t lba_addr, CHS_t* chs_addr);
lbaint_t chstolba(const chs_disk_geometry_t* desc, const CHS_t* chs_addr);
void chs_disk_geometry_print(const chs_disk_geometry_t* geometry);


typedef struct mem_area_s {
    uint32_t block_addr;
    size_t block_size;
    uint32_t block_offset;
} mem_area_t;

typedef struct block_drv_op_s {
    int (*block_drv_open)(minor_t minor);
    ssize_t (*block_drv_read)(minor_t minor, lbaint_t lba_start, lbaint_t count, void* buffer);
    ssize_t (*block_drv_write)(minor_t minor, lbaint_t lba_start, lbaint_t count, void* buffer);
    int (*block_drv_close)(minor_t minor);
} block_drv_op_t;

typedef struct {
    major_t major;
    block_drv_op_t ops;
} block_dev_driver_t;

extern int hd_open(minor_t minor);
extern ssize_t hd_read(minor_t minor, lbaint_t lba_start, lbaint_t count, void* buffer);
extern ssize_t hd_write(minor_t minor, lbaint_t lba_start, lbaint_t count, void* buffer);
extern int hd_close(minor_t minor);

block_drv_op_t* block_dev_driver_lookuptable(major_t major);

#endif
