#ifndef _GDT_H
#define _GDT_H

#include <arch/x86/checkCCversion.h>
#include <arch/x86/regs.h>

#include <stdint.h>

#define GDT_SEG_UNUSED    0
#define GDT_SEG_ACCESSED  1

#define GDT_SEG_SYSTEM_DESC    0
#define GDT_SEG_DATA_CODE_DESC 1

#define GDT_CODE_SEG_EXECUTE_ONLY 0
#define GDT_CODE_SEG_EXECUTE_READ 1
#define GDT_CODE_SEG_NONCONFORMING 0
#define GDT_CODE_SEG_CONFORMING    1
#define GDT_CODE_SEG_16BIT_MODE 0
#define GDT_CODE_SEG_32BIT_MODE 1

#define GDT_STACK_SEG_16BIT_MODE 0
#define GDT_STACK_SEG_32BIT_MODE 1

#define GDT_DATA_SEG_READ_ONLY  0
#define GDT_DATA_SEG_READ_WRITE 1
#define GDT_DATA_SEG_EXPAND_UP    0
#define GDT_DATA_SEG_EXPAND_DOWN  1
#define GDT_DATA_SEG_UPPER_BOUND_64KB 0
#define GDT_DATA_SEG_UPPER_BOUND_4GB  1

#define GDT_SEG_TYPE_DATA 0
#define GDT_SEG_TYPE_CODE 1

#define GDT_SEG_PRIVILEGE_RING_0 0
#define GDT_SEG_PRIVILEGE_RING_1 1
#define GDT_SEG_PRIVILEGE_RING_2 2
#define GDT_SEG_PRIVILEGE_RING_3 3

#define GDT_SEG_GRANULARITY_BYTE 0
#define GDT_SEG_GRANULARITY_4KB  1

#define GDT_SEG_ABSENT_IN_MEMORY  0
#define GDT_SEG_PRESENT_IN_MEMORY 1

#define GDT_SEG_COMPATIBILITY_MODE 0
#define GDT_SEG_64BIT_MODE         1


typedef struct {
    uint8_t accessed : 1; // 0: unused, 1: accessed/used
    union {
        struct {
            uint8_t readable : 1; // 0: execute only, 1: execute/read
            uint8_t conforming : 1; // 0: not conforming (can only be called from a segment with the same priviledge),
                                    // 1: conforming (can be called from a segment with higher priviledge)
        };
        struct {
            uint8_t writable : 1; // 0: read only, 1: read/write
            uint8_t expansion_direction : 1; // 0: expand up (classic), 1: expand down (as in a stack)
        };
    };
    uint8_t executable : 1; // 0: data segment, 1: code segment
} __attribute__ ((packed)) gdt_descriptor_type_t;

/* GDT's segment descriptor */
typedef struct {

    uint16_t limit_15_0;

    uint16_t base_15_0;

    uint8_t base_23_16;

    union {
        struct {
uint8_t : 1;
          uint8_t readable : 1; // 0: execute only, 1: execute/read
          uint8_t conforming : 1; // 0: not conforming (can only be called from a segment with the same priviledge),
          // 1: conforming (can be called from a segment with higher priviledge)
uint8_t : 5;
        };
        struct {
uint8_t : 1;
          uint8_t writable : 1; // 0: read only, 1: read/write
          uint8_t expansion_direction : 1; // 0: expand up (classic), 1: expand down (as in a stack)
uint8_t : 5;
        };
        struct {
            uint8_t accessed : 1; // 0: unused, 1: accessed/used
uint8_t : 2;
          uint8_t executable : 1; // 0: data segment, 1: code segment

          // 0: system segment descriptor, 1: code or data descriptor
          uint8_t descriptor_type : 1;
          // 00 : highest privilege, 11: lowest privilege
          uint8_t privilege_level : 2;
          // 0: absent, 1: segment present in memory
          uint8_t present : 1;
        };
    };

    union {
        struct {
            uint8_t limit_19_16 : 4;

            uint8_t AVL : 1; // reserved
            // 0: for 32-bit segments (compatibility mode), 1: 64-bit mode
            uint8_t L : 1;
uint8_t : 1;
          // 0: segment limit interpreted in byte, 1: in 4KB unit
          uint8_t granularity : 1;
        };
        struct {
uint8_t : 6;
            // 0: 16-bit addresses and 16-bit and 8-bit operands,
            // 1: 32-bit adresses and 32-bit and 8-bit operands
            uint8_t default_op_size : 1;
uint8_t : 1;
        };
        struct {
uint8_t : 6;
          // 0 : 16-bit stack pointer, 1 : 32-bit stack pointer
          uint8_t default_sp_size : 1;
uint8_t : 1;
        };
        struct {
uint8_t : 6;
            // 0: upper bound is 0xFFFF (64KB), 1: 0xFFFFFFFF (4GB)
            uint8_t upper_bound : 1;
uint8_t : 1;
        };
    };

    uint8_t base_31_24;

} __attribute__ ((packed)) gdt_descriptor_t;

/* Registre GDTR */
typedef struct {
        uint16_t limit;
        uint32_t base;
} __attribute__ ((packed)) gdtr_t;

/*
 * Global descriptor table 32-bit
 * [0] : null descriptor
 * [1] : 32-bit protected mode's code segment's descriptor
 * [2] : 32-bit protected mode's data segment's descriptor
 * */
#define GDT32_NB_DESCRIPTORS 3
#define GDT32_INDEX_CODE_SEG 1
#define GDT32_INDEX_DATA_SEG 2

/*
 * Global descriptor table 16-bit
 * [0] : null descriptor
 * [1] : 16-bit protected mode's code segment's descriptor
 * [2] : 16-bit protected mode's data segment's descriptor
 * */
#define GDT16_NB_DESCRIPTORS 3
#define GDT16_INDEX_CODE_SEG 1
#define GDT16_INDEX_DATA_SEG 2

void i86_gdt_init(void);

#endif
