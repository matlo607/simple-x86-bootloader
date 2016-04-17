#include "gdt.h"

gdtr_t GDTR_val;
gdt_descriptor_t GDT[3];

void gdt_descriptor_set_base(gdt_descriptor_t* p_gdt_desc, uint32_t base)
{
    p_gdt_desc->base_15_0 = (uint16_t)(base & 0x0000FFFF);
    p_gdt_desc->base_23_16 = (uint8_t)((base >> 16) & 0x00FF);
    p_gdt_desc->base_31_24 = (uint8_t)(base >> 24);
}

void gdt_descriptor_set_limit(gdt_descriptor_t* p_gdt_desc, uint32_t limit)
{
    p_gdt_desc->limit_15_0 = (uint16_t)(limit & 0x0000FFFF);
    p_gdt_desc->limit_19_16 = (uint8_t)((limit >> 16) & 0x000F);
}
