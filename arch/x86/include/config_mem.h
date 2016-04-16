#ifndef _CONFIG_MEM_H
#define _CONFIG_MEM_H

#define SEGMENT_CODE         0x0000

#define SEGMENT_DATA         0x1000
#define DATA_BASE_OFFSET     0

#define SEGMENT_EXTRA_DATA   0x2000

/* Stack segment (SEGMENT_STACK:STACK_BASE_OFFSET) = (1000:FFFF) --> (1000:EFFF) */
#define SEGMENT_STACK        0x1000
#define STACK_BASE_OFFSET    0xffff

#endif
