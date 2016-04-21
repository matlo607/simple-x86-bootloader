#ifndef _CONFIG_MEM_H
#define _CONFIG_MEM_H

#define SEGMENT_CODE         0x0000

#define SEGMENT_DATA         0x1000
#ifndef BOOTLOADER_PROTECTED_MODE_ENABLED
#define DATA_BASE_OFFSET     0x0000
#else
#define DATA_BASE_OFFSET     0x10000
#endif

#define SEGMENT_EXTRA_DATA   0x2000

/* Stack segment (SEGMENT_STACK:STACK_BASE_OFFSET) = (1000:FFFF) --> (1000:EFFF) */
#define SEGMENT_STACK        0x1000
#define STACK_BASE_OFFSET    0xffff

#endif
