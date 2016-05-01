#ifndef _DEV_H
#define _DEV_H

#include <stdint.h>
#include <stddef.h>

typedef int8_t devstatus_t;

#define DEV_STATUS_NOT_FOUND    -1
#define DEV_STATUS_UNKNOWN      0
#define DEV_STATUS_INITIALIZED  1
#define DEV_STATUS_OPENED       2
#define DEV_STATUS_CLOSED       3

typedef uint8_t major_t;

#define FLOPPY_DISK_DRIVER 0
#define HARD_DISK_DRIVER 1

typedef uint8_t minor_t;

typedef struct {
    const char* name;
    major_t major;
    minor_t minor;
    devstatus_t status;
    void* info;
} dev_t;

devstatus_t dev_getstatus(major_t major, minor_t minor);
devstatus_t dev_setstatus(major_t major, minor_t minor, devstatus_t status);

void* dev_getinfo(major_t major, minor_t minor);
void* dev_setinfo(major_t major, minor_t minor, void* info);

dev_t* dev_lookuptable(const char* name);

#endif
