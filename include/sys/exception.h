#ifndef _EXCEPTION_H
#define _EXCEPTION_H

// divide by 0
void divide_by_zero_fault(void);

// single step
void single_step_trap(void);

// non maskable interrupt trap
void nmi_trap(void);

// breakpoint hit
void breakpoint_trap(void);

// overflow
void overflow_trap(void);

// bounds check
void bounds_check_fault(void);

// invalid opcode / instruction
void invalid_opcode_fault(void);

// device not available
void device_availibility_fault(void);

// double fault
void double_fault_abort(void);

// invalid Task State Segment (TSS)
void invalid_tss_fault(void);

// segment not present
void no_segment_fault(void);

// stack fault
void stack_fault(void);

// general protection fault
void general_protection_fault(void);

// page fault
void page_fault(void);

// Floating Point Unit (FPU) error
void fpu_fault(void);

// alignment check
void alignment_check_fault(void);

// machine check
void machine_check_abort(void);

// Floating Point Unit (FPU) Single Instruction Multiple Data (SIMD) error
void simd_fpu_fault(void);

#endif
