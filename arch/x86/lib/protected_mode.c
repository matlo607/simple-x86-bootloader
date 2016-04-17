#include <gdt.h>
#include <cpu.h>

#include <string.h>

void switch_to_protected_mode(void)
{
    // clear the structures
    memset(&GDTR_val, 0, sizeof(gdtr_t));
    memset(&GDT, 0, sizeof(gdt_descriptor_t) * GDT_NB_DESCRIPTORS);

    // set correctly the segment descriptors

    // code segment descriptor
    // -----------------------
    GDT[1].type.accessed = GDT_SEG_ACCESSED;
    GDT[1].type.readable = GDT_CODE_SEG_EXECUTE_READ;
    GDT[1].type.conforming = GDT_CODE_SEG_NONCONFORMING;
    GDT[1].type.executable = GDT_SEG_TYPE_CODE;

    GDT[1].descriptor_type = GDT_SEG_DATA_CODE_DESC;
    GDT[1].privilege_level = GDT_SEG_PRIVILEGE_RING_0;
    GDT[1].present = GDT_SEG_PRESENT_IN_MEMORY;

    GDT[1].AVL = 1;
    GDT[1].L = GDT_SEG_COMPATIBILITY_MODE;
    GDT[1].default_op_size = GDT_CODE_SEG_32BIT_MODE;
    GDT[1].granularity = GDT_SEG_GRANULARITY_4KB;

    // base : the minimum (0x0000000)
    // limit : the maximum (4GB)
    gdt_descriptor_set_base(&GDT[1], 0x00000000);
    gdt_descriptor_set_limit(&GDT[1], 0xFFFFF);


    // data segment descriptor
    // -----------------------
    GDT[2].type.accessed = GDT_SEG_ACCESSED;
    GDT[2].type.readable = GDT_DATA_SEG_READ_WRITE;
    GDT[2].type.conforming = GDT_DATA_SEG_EXPAND_UP;
    GDT[2].type.executable = GDT_SEG_TYPE_DATA;

    GDT[2].descriptor_type = GDT_SEG_DATA_CODE_DESC;
    GDT[2].privilege_level = GDT_SEG_PRIVILEGE_RING_0;
    GDT[2].present = GDT_SEG_PRESENT_IN_MEMORY;

    GDT[2].AVL = 1;
    GDT[2].L = GDT_SEG_COMPATIBILITY_MODE;
    GDT[2].upper_bound = GDT_DATA_SEG_UPPER_BOUND_4GB;
    GDT[2].granularity = GDT_SEG_GRANULARITY_4KB;

    // base : the minimum (0x0000000)
    // limit : the maximum (4GB)
    gdt_descriptor_set_base(&GDT[2], 0x00000000);
    gdt_descriptor_set_limit(&GDT[2], 0xFFFFF);


    uint16_t reg_ds;
    reg_get_data_segment(&reg_ds);
    GDTR_val.base = (((uint32_t)reg_ds) << 4) | ((uint32_t)&GDT);
    GDTR_val.limit = sizeof(gdt_descriptor_t) * GDT_NB_DESCRIPTORS;

    // load the GDT pointer
    GDTR_set(&GDTR_val);

    // switch to protected mode by setting the bit 0 of CR0 to 1
    uint32_t cr0;
    CR0_get(&cr0);
    cr0 |= CR0_BIT0_PROTECTED_MODE;
    CR0_set(cr0);
}

void switch_to_real_mode(void)
{
    
}
