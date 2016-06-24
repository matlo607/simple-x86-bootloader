#include "sys/exception.h"

#include <sys/system.h>

void divide_by_zero_fault(void)
{
    panic("exception(fault): divide by zero");
}

void single_step_trap(void)
{
    panic("exception(trap): single step");
}

void nmi_trap(void)
{
    panic("exception(trap): non maskable interrupt");
}

void breakpoint_trap(void)
{
    panic("exception(trap): breakpoint");
}

void overflow_trap(void)
{
    panic("exception(trap): overflow");
}

void bounds_check_fault(void)
{
    panic("exception(fault): bounds check failed");
}

void invalid_opcode_fault(void)
{
    panic("exception(fault): invalid opcode / instruction");
}

void device_availibility_fault(void)
{
    panic("exception(fault): device not available");
}

void double_fault_abort(void)
{
    panic("exception(abort): double fault");
}

void invalid_tss_fault(void)
{
    panic("exception(fault): invalid TSS");
}

void no_segment_fault(void)
{
    panic("exception(fault): segment not present");
}

void stack_fault(void)
{
    panic("exception(fault): stack fault");
}

void general_protection_fault(void)
{
    panic("exception(fault): general protection fault");
}

void page_fault(void)
{
    panic("exception(fault): page fault");
}

void fpu_fault(void)
{
    panic("exception(fault): FPU error");
}

void alignment_check_fault(void)
{
    panic("exception(fault): alignment check failed");
}

void machine_check_abort(void)
{
    panic("exception(abort): machine check aborted");
}

void simd_fpu_fault(void)
{
    panic("exception(fault): FPU SIMD error");
}
