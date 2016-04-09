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
#define reg_set_data_segment(ADDR) \
    __asm__ __volatile__ ( "movw %w0, %%ds;" \
                           : \
                           : "r" (ADDR) )

#define reg_get_data_segment(ptr_val_ds) \
    __asm__ __volatile__ ( "movw %%ds, %%ax;" \
                           : "=a" (*ptr_val_ds) \
                           : \
                           : )

#define reg_set_extended_data_segment(ADDR) \
    __asm__ __volatile__ ( "movw %w0, %%es;" \
                           : \
                           : "r" (ADDR) )

#define reg_get_extended_data_segment(ptr_val_es) \
    __asm__ __volatile__ ( "movw %%es, %%ax;" \
                           : "=a" (*ptr_val_es) \
                           : \
                           : )

#define reg_get_code_segment(ptr_val_cs) \
    __asm__ __volatile__ ( "movw %%cs, %%ax;" \
                           : "=a" (*ptr_val_cs) \
                           : \
                           : )

#define reg_get_stack_segment(ptr_val_ss) \
    __asm__ __volatile__ ( "movw %%ss, %%ax;" \
                           : "=a" (*ptr_val_ss) \
                           : \
                           : )

#define reg_set_stack_segment(ADDR) \
    __asm__ __volatile__ ( "movw %w0, %%ss" : : "r" (ADDR) )

#define reg_set_stack_pointer(ADDR) \
    __asm__ __volatile__ ( "movw %w0, %%sp" : : "r" (ADDR) )

#define reg_set_stack_base_pointer(ADDR) \
    __asm__ __volatile__ ( "movw %w0, %%bp" : : "r" (ADDR) )

#endif
