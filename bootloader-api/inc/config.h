#ifndef _CONFIG
#define _CONFIG

/*
 * Software information
 * */
#define _STR_release_mode_alpha "alpha"
#define _STR_release_mode_beta  "beta"
#define _STR_release_mode_RC    "RC"

#define BOOTLOADER_RELEASE_MODE   _STR_release_mode_alpha
#define BOOTLOADER_VERSION        0
#define BOOTLOADER_REVISION       1


/*
 * Memory configuration
 * */
#define HEAP_MAX_SIZE           4096 // 4kB

/*
 * Debug params
 * */
#define DEBUG

#ifdef DEBUG
    #define DEBUG_ON    true

    //#define DEBUG_HEAP
#endif

#endif
