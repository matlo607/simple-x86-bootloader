#ifndef _CPU_H
#define _CPU_H

#define interrupts_enable() \
    __asm__ __volatile__ ( "sti" )

#define interrupts_disable() \
    __asm__ __volatile__ ( "cli" )

#define halt_cpu() \
    __asm__ __volatile__ ( "hlt" )

/*************
 * Registers *
 *************/

#define CR0_BIT0_PROTECTED_MODE (1)
#define CR0_BIT31_PAGING        (1 << 30)

#define CR0_get(ptr_cr0_val) \
    __asm__ __volatile__ ( "movl %%cr0, %[ptr];" \
                           : [ptr] "=r" (*ptr_cr0_val) \
                           : )

#define CR0_set(cr0_val) \
    __asm__ __volatile__ ( "movl %0, %%cr0;" \
                           : \
                           : "r" (cr0_val))


#define GDTR_set(gdtr_val) \
    __asm__ __volatile__ ( "lgdt (%[gdtr]);" \
                           : \
                           : [gdtr] "r" (gdtr_val) )


#define reg_set_data_segment(ADDR) \
    __asm__ __volatile__ ( "movw %w0, %%ds;" \
                           : \
                           : "r" (ADDR) )

#define reg_get_data_segment(ptr_val_ds) \
    __asm__ __volatile__ ( "movw %%ds, %[ptr];" \
                           : [ptr] "=r" (*ptr_val_ds) \
                           : \
                           : )

#define reg_set_extended_data_segment(ADDR) \
    __asm__ __volatile__ ( "movw %w0, %%es;" \
                           : \
                           : "r" (ADDR) )

#define reg_get_extended_data_segment(ptr_val_es) \
    __asm__ __volatile__ ( "movw %%es, %w0;" \
                           : "=r" (*ptr_val_es) \
                           : \
                           : )

#define reg_get_code_segment(ptr_val_cs) \
    __asm__ __volatile__ ( "movw %%cs, %%w0;" \
                           : "=r" (*ptr_val_cs) \
                           : \
                           : )

#define reg_get_stack_segment(ptr_val_ss) \
    __asm__ __volatile__ ( "movw %%ss, %%w0;" \
                           : "=r" (*ptr_val_ss) \
                           : \
                           : )

#define reg_set_stack_segment(ADDR) \
    __asm__ __volatile__ ( "movw %w0, %%ss" : : "r" (ADDR) )

#define reg_set_stack_pointer(ADDR) \
    __asm__ __volatile__ ( "movw %w0, %%sp" : : "r" (ADDR) )

#define reg_set_stack_base_pointer(ADDR) \
    __asm__ __volatile__ ( "movw %w0, %%bp" : : "r" (ADDR) )

#endif
