#include "arch/x86/gdt.h"
#include <arch/x86/config_mem.h>

#include <debug.h>
#include <string.h>
#include <stdio.h>
#include <arch/x86/cpu.h>
#include <arch/x86/cpu_mode_switching.h>

static gdt_descriptor_t GDT16[GDT16_NB_DESCRIPTORS];
static gdt_descriptor_t GDT32[GDT32_NB_DESCRIPTORS];

static void gdt_descriptor_set_base(gdt_descriptor_t* p_gdt_desc, uint32_t base);
static void gdt_descriptor_set_limit(gdt_descriptor_t* p_gdt_desc, uint32_t limit);

static void i86_gdt16_init(void);
static void i86_gdt32_init(void);

void i86_gdt_init(void)
{
    i86_gdt16_init();
    i86_gdt32_init();

    // load the 32-bit GDT's pointer
    lgdt(&cntxt_p_mode_32bit.ptr_gdt);
#ifdef DEBUG_GDT
    debug_printf("GDT: rgdt=%#08x\r\n", &cntxt_p_mode_32bit.ptr_gdt);
#endif

    __asm__ __volatile__ ( "movw %[gdt_offset_data_descriptor], %%ax;" \
                           "movw %%ax, %%ds;" \
                           "movw %%ax, %%es;" \
                           "movw %%ax, %%fs;" \
                           "movw %%ax, %%gs;" \
                           "movw %%ax, %%ss;" \
                           "ljmp %[gdt_offset_code_descriptor], $_gdt_init_next;" \
                           "_gdt_init_next:" \
                           : \
                           : [gdt_offset_data_descriptor] "i" (GDT32_INDEX_DATA_SEG * sizeof(gdt_descriptor_t)),
                             [gdt_offset_code_descriptor] "i" (GDT32_INDEX_CODE_SEG * sizeof(gdt_descriptor_t))
                           : "eax");
}

static void i86_gdt32_init(void)
{
    memset(&GDT32, 0, sizeof(gdt_descriptor_t) * GDT32_NB_DESCRIPTORS);

    // set correctly the segment descriptors

    // code segment descriptor - 32bit protected mode
    // ----------------------------------------------
    GDT32[GDT32_INDEX_CODE_SEG].accessed = GDT_SEG_UNUSED;
    GDT32[GDT32_INDEX_CODE_SEG].readable = GDT_CODE_SEG_EXECUTE_READ;
    GDT32[GDT32_INDEX_CODE_SEG].conforming = GDT_CODE_SEG_NONCONFORMING;
    GDT32[GDT32_INDEX_CODE_SEG].executable = GDT_SEG_TYPE_CODE;

    GDT32[GDT32_INDEX_CODE_SEG].descriptor_type = GDT_SEG_DATA_CODE_DESC;
    GDT32[GDT32_INDEX_CODE_SEG].privilege_level = GDT_SEG_PRIVILEGE_RING_0;
    GDT32[GDT32_INDEX_CODE_SEG].present = GDT_SEG_PRESENT_IN_MEMORY;

    GDT32[GDT32_INDEX_CODE_SEG].AVL = 0;
    GDT32[GDT32_INDEX_CODE_SEG].L = GDT_SEG_COMPATIBILITY_MODE;
    GDT32[GDT32_INDEX_CODE_SEG].default_op_size = GDT_CODE_SEG_32BIT_MODE;
    GDT32[GDT32_INDEX_CODE_SEG].granularity = GDT_SEG_GRANULARITY_4KB;

    // base : the minimum (0x0000000)
    // limit : the maximum (4GB)
    gdt_descriptor_set_base (&GDT32[GDT32_INDEX_CODE_SEG], 0x00000000);
    gdt_descriptor_set_limit(&GDT32[GDT32_INDEX_CODE_SEG], 0xFFFFF);
#ifdef DEBUG_GDT
    debug_printf("GDT: 32bit code segment: base=0x00000000, limit=0xFFFFF (4GB)\r\n");
#endif


    // data segment descriptor - 32bit protected mode
    // ----------------------------------------------
    GDT32[GDT32_INDEX_DATA_SEG].accessed = GDT_SEG_UNUSED;
    GDT32[GDT32_INDEX_DATA_SEG].readable = GDT_DATA_SEG_READ_WRITE;
    GDT32[GDT32_INDEX_DATA_SEG].conforming = GDT_DATA_SEG_EXPAND_UP;
    GDT32[GDT32_INDEX_DATA_SEG].executable = GDT_SEG_TYPE_DATA;

    GDT32[GDT32_INDEX_DATA_SEG].descriptor_type = GDT_SEG_DATA_CODE_DESC;
    GDT32[GDT32_INDEX_DATA_SEG].privilege_level = GDT_SEG_PRIVILEGE_RING_0;
    GDT32[GDT32_INDEX_DATA_SEG].present = GDT_SEG_PRESENT_IN_MEMORY;

    GDT32[GDT32_INDEX_DATA_SEG].AVL = 0;
    GDT32[GDT32_INDEX_DATA_SEG].L = GDT_SEG_COMPATIBILITY_MODE;
    GDT32[GDT32_INDEX_DATA_SEG].upper_bound = GDT_DATA_SEG_UPPER_BOUND_4GB;
    GDT32[GDT32_INDEX_DATA_SEG].granularity = GDT_SEG_GRANULARITY_4KB;

    // base : the minimum (0x0000000)
    // limit : the maximum (4GB)
    gdt_descriptor_set_base(&GDT32[GDT32_INDEX_DATA_SEG], 0x00000000);
    gdt_descriptor_set_limit(&GDT32[GDT32_INDEX_DATA_SEG], 0xFFFFF);
#ifdef DEBUG_GDT
    debug_printf("GDT: 32bit data segment: base=0x00000000, limit=0xFFFFF (4GB)\r\n");
#endif

    // set the registers to point on the good segment descriptors
    cntxt_p_mode_32bit.ptr_gdt.base = (uint32_t) &GDT32;
    cntxt_p_mode_32bit.ptr_gdt.limit = sizeof(gdt_descriptor_t) * GDT32_NB_DESCRIPTORS;


    cntxt_p_mode_32bit.regs.CS = GDT32_INDEX_CODE_SEG * sizeof(gdt_descriptor_t);
    cntxt_p_mode_32bit.regs.DS = GDT32_INDEX_DATA_SEG * sizeof(gdt_descriptor_t);
    cntxt_p_mode_32bit.regs.ES = GDT32_INDEX_DATA_SEG * sizeof(gdt_descriptor_t);
    cntxt_p_mode_32bit.regs.FS = GDT32_INDEX_DATA_SEG * sizeof(gdt_descriptor_t);
    cntxt_p_mode_32bit.regs.GS = GDT32_INDEX_DATA_SEG * sizeof(gdt_descriptor_t);
    cntxt_p_mode_32bit.regs.SS = GDT32_INDEX_DATA_SEG * sizeof(gdt_descriptor_t);
    cntxt_p_mode_32bit.regs.SP._eR = STACK_BASE_OFFSET;

#ifdef DEBUG_GDT
    debug_printf("GDT: %%cs=%#x, %%ds=%#x, %%es=%#x, %%fs=%#x, %%gs=%#x, %%ss=%#x\r\n",
            cntxt_p_mode_32bit.regs.CS,
            cntxt_p_mode_32bit.regs.DS,
            cntxt_p_mode_32bit.regs.ES,
            cntxt_p_mode_32bit.regs.FS,
            cntxt_p_mode_32bit.regs.GS,
            cntxt_p_mode_32bit.regs.SS);
#endif
}

static void i86_gdt16_init(void)
{
    memset(&GDT16, 0, sizeof(gdt_descriptor_t) * GDT16_NB_DESCRIPTORS);

    // set correctly the segment descriptors

    // code segment descriptor - 16bit protected mode
    // ----------------------------------------------
    GDT16[GDT16_INDEX_CODE_SEG].accessed = GDT_SEG_UNUSED;
    GDT16[GDT16_INDEX_CODE_SEG].readable = GDT_CODE_SEG_EXECUTE_READ;
    GDT16[GDT16_INDEX_CODE_SEG].conforming = GDT_CODE_SEG_NONCONFORMING;
    GDT16[GDT16_INDEX_CODE_SEG].executable = GDT_SEG_TYPE_CODE;

    GDT16[GDT16_INDEX_CODE_SEG].descriptor_type = GDT_SEG_DATA_CODE_DESC;
    GDT16[GDT16_INDEX_CODE_SEG].privilege_level = GDT_SEG_PRIVILEGE_RING_0;
    GDT16[GDT16_INDEX_CODE_SEG].present = GDT_SEG_PRESENT_IN_MEMORY;

    GDT16[GDT16_INDEX_CODE_SEG].AVL = 0;
    GDT16[GDT16_INDEX_CODE_SEG].L = GDT_SEG_COMPATIBILITY_MODE;
    GDT16[GDT16_INDEX_CODE_SEG].default_op_size = GDT_CODE_SEG_16BIT_MODE;
    GDT16[GDT16_INDEX_CODE_SEG].granularity = GDT_SEG_GRANULARITY_BYTE;

    // base : the minimum (0x00000)
    // limit : the maximum (1MB)
    gdt_descriptor_set_base(&GDT16[GDT16_INDEX_CODE_SEG], 0x00000000);
    gdt_descriptor_set_limit(&GDT16[GDT16_INDEX_CODE_SEG], 0xFFFFF);

    // data segment descriptor - 16bit protected mode
    // ----------------------------------------------
    GDT16[GDT16_INDEX_DATA_SEG].accessed = GDT_SEG_UNUSED;
    GDT16[GDT16_INDEX_DATA_SEG].readable = GDT_DATA_SEG_READ_WRITE;
    GDT16[GDT16_INDEX_DATA_SEG].conforming = GDT_DATA_SEG_EXPAND_UP;
    GDT16[GDT16_INDEX_DATA_SEG].executable = GDT_SEG_TYPE_DATA;

    GDT16[GDT16_INDEX_DATA_SEG].descriptor_type = GDT_SEG_DATA_CODE_DESC;
    GDT16[GDT16_INDEX_DATA_SEG].privilege_level = GDT_SEG_PRIVILEGE_RING_0;
    GDT16[GDT16_INDEX_DATA_SEG].present = GDT_SEG_PRESENT_IN_MEMORY;

    GDT16[GDT16_INDEX_DATA_SEG].AVL = 0;
    GDT16[GDT16_INDEX_DATA_SEG].L = GDT_SEG_COMPATIBILITY_MODE;
    GDT16[GDT16_INDEX_DATA_SEG].upper_bound = GDT_DATA_SEG_UPPER_BOUND_64KB;
    GDT16[GDT16_INDEX_DATA_SEG].granularity = GDT_SEG_GRANULARITY_BYTE;

    // base : the minimum (0x00000)
    // limit : the maximum (1MB)
    gdt_descriptor_set_base(&GDT16[GDT16_INDEX_DATA_SEG], 0x00000000);
    gdt_descriptor_set_limit(&GDT16[GDT16_INDEX_DATA_SEG], 0xFFFFF);

    // set the registers to point on the good segment descriptors
    cntxt_p_mode_16bit.ptr_gdt.base = (uint32_t) &GDT16;
    cntxt_p_mode_16bit.ptr_gdt.limit = sizeof(gdt_descriptor_t) * GDT16_NB_DESCRIPTORS;


    cntxt_p_mode_16bit.regs.CS = GDT16_INDEX_CODE_SEG * sizeof(gdt_descriptor_t);
    cntxt_p_mode_16bit.regs.DS = GDT16_INDEX_DATA_SEG * sizeof(gdt_descriptor_t);
    cntxt_p_mode_16bit.regs.ES = GDT16_INDEX_DATA_SEG * sizeof(gdt_descriptor_t);
    cntxt_p_mode_16bit.regs.FS = GDT16_INDEX_DATA_SEG * sizeof(gdt_descriptor_t);
    cntxt_p_mode_16bit.regs.GS = GDT16_INDEX_DATA_SEG * sizeof(gdt_descriptor_t);
    cntxt_p_mode_16bit.regs.SS = GDT16_INDEX_DATA_SEG * sizeof(gdt_descriptor_t);
    cntxt_p_mode_16bit.regs.SP._R = 0xFFFF;
}

static void gdt_descriptor_set_base(gdt_descriptor_t* p_gdt_desc, uint32_t base)
{
    p_gdt_desc->base_15_0 = (uint16_t)(base & 0x0000FFFF);
    p_gdt_desc->base_23_16 = (uint8_t)((base >> 16) & 0x00FF);
    p_gdt_desc->base_31_24 = (uint8_t)(base >> 24);
}

static void gdt_descriptor_set_limit(gdt_descriptor_t* p_gdt_desc, uint32_t limit)
{
    p_gdt_desc->limit_15_0 = (uint16_t)(limit & 0x0000FFFF);
    p_gdt_desc->limit_19_16 = (uint8_t)((limit >> 16) & 0x000F);
}
