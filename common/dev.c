#include "dev.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>

#define DEVICE_NB 2
static dev_t list_dev[DEVICE_NB] =
{
    {
        .name = "hda",
        .major = HARD_DISK_DRIVER,
        .minor = 0,
        .status = DEV_STATUS_UNKNOWN,
        .info = NULL
    },
    {
        .name = "fda",
        .major = FLOPPY_DISK_DRIVER,
        .minor = 0,
        .status = DEV_STATUS_UNKNOWN,
        .info = NULL
    }
};

static void dev_printtable(void)
{
    for (uint8_t dev_index = 0; dev_index < DEVICE_NB; dev_index++) {
        debug_printf("name = %s, (major, minor) = (%u, %u)\r\n", list_dev[dev_index].name, list_dev[dev_index].major, list_dev[dev_index].minor);
    }
}

devstatus_t dev_getstatus(major_t major, minor_t minor)
{
    for (uint8_t dev_index = 0; dev_index < DEVICE_NB; dev_index++) {
        if (list_dev[dev_index].major == major && list_dev[dev_index].minor == minor) {
            return list_dev[dev_index].status;
        }
    }

    return DEV_STATUS_NOT_FOUND;
}

devstatus_t dev_setstatus(major_t major, minor_t minor, devstatus_t status) {
    for (uint8_t dev_index = 0; dev_index < DEVICE_NB; dev_index++) {
        if (list_dev[dev_index].major == major && list_dev[dev_index].minor == minor) {
            list_dev[dev_index].status = status;
            return status;
        }
    }

    return DEV_STATUS_NOT_FOUND;
}

void* dev_getinfo(major_t major, minor_t minor)
{
    for (uint8_t dev_index = 0; dev_index < DEVICE_NB; dev_index++) {
        if (list_dev[dev_index].major == major && list_dev[dev_index].minor == minor) {
            return list_dev[dev_index].info;
        }
    }

    return NULL;
}

void* dev_setinfo(major_t major, minor_t minor, void* info)
{
    for (uint8_t dev_index = 0; dev_index < DEVICE_NB; dev_index++) {
        if (list_dev[dev_index].major == major && list_dev[dev_index].minor == minor) {
            list_dev[dev_index].info = info;
            return info;
        }
    }

    return NULL;
}

dev_t* dev_lookuptable(const char* name)
{
    assert(name != NULL);
    debug_printf("device : %s\r\n", name);
    dev_printtable();

    for (uint8_t dev_index = 0; dev_index < DEVICE_NB; dev_index++) {
        debug_printf("compare with device : %s\r\n", list_dev[dev_index].name);
        if (strcmp(list_dev[dev_index].name, name) == 0) {
            return &list_dev[dev_index];
        }
    }

    return NULL;
}
