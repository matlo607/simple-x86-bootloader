#include "sys/hal.h"

#include <arch/x86/cpu.h>
#include <arch/x86/pic.h>
#include <arch/x86/idt.h>
#include <arch/x86/clock.h>
#include <arch/x86/keyboard.h>

#include <sys/heap.h>
#include <sys/exception.h>
#include <sys/input.h>

#include <stdio.h>
#include <string.h>

static void i86_setup_exception_handlers(void);

void hal_init(void)
{
    i86_cpu_init();

    i86_pic_remap(IDT32_PIC1_INT_VECT_OFFSET, IDT32_PIC2_INT_VECT_OFFSET, MASK_DISABLE_ALL_INTERRUPTS);

    i86_setup_exception_handlers();

    // The heap can be necessary for the initialization of some devices
    heap_init();

    // At this point, we have set the interrupt handlers and parametered the PIC.
    // The next step is to initialize all the peripherals
    i86_clock_init();
    i86_softwaretimer_init();

    keyboard_init();
    input_initialize();

    // enable interrupts
    sti();
}

void hal_shutdown(void)
{
    // disable interrupts
    cli();

    i86_cpu_shutdown();
}

void hal_geninterrupt(uint8_t intno)
{
    __asm__ __volatile__ (
      "movb %[intno], %%al ;"
      "leal 0f, %%ebx ;"
      "movb %%al, 1(%%ebx) ;"
      "jmp 0f ;"
      "0: ;"
      "int $0 ;"
      :
      : [intno] "g" (intno)
      : "%eax", "%ebx");
}

extern void _asm_divide_by_zero_fault(void);
extern void _asm_single_step_trap(void);
extern void _asm_nmi_trap(void);
extern void _asm_breakpoint_trap(void);
extern void _asm_overflow_trap(void);
extern void _asm_bounds_check_fault(void);
extern void _asm_invalid_opcode_fault(void);
extern void _asm_device_availibility_fault(void);
extern void _asm_double_fault_abort(void);
extern void _asm_invalid_tss_fault(void);
extern void _asm_no_segment_fault(void);
extern void _asm_stack_fault(void);
extern void _asm_general_protection_fault(void);
extern void _asm_page_fault(void);
extern void _asm_fpu_fault(void);
extern void _asm_alignment_check_fault(void);
extern void _asm_machine_check_abort(void);
extern void _asm_simd_fpu_fault(void);

static void i86_setup_exception_handlers(void)
{
    idt_descriptor_flags_t flags;
    memset(&flags, 0, sizeof(idt_descriptor_flags_t));

    flags.gate_type = IDT_DESC_TYPE_32BIT_INTERRUPT_GATE;
    flags.privilege_level = IDT_DESC_PRIVILEGE_RING_0;
    flags.present = IDT_DESC_PRESENT_IN_MEMORY;

    i86_idt_install_ir(IDT32_EXCEPTIONS_INT_OFFSET(DIV0INT), _asm_divide_by_zero_fault, flags);
    i86_idt_install_ir(IDT32_EXCEPTIONS_INT_OFFSET(SINGLESTEPINT), _asm_single_step_trap, flags);
    i86_idt_install_ir(IDT32_EXCEPTIONS_INT_OFFSET(NMIINT), _asm_nmi_trap, flags);
    i86_idt_install_ir(IDT32_EXCEPTIONS_INT_OFFSET(BREAKPOINTINT), _asm_breakpoint_trap, flags);
    i86_idt_install_ir(IDT32_EXCEPTIONS_INT_OFFSET(OVERFLOWINT), _asm_overflow_trap, flags);
    i86_idt_install_ir(IDT32_EXCEPTIONS_INT_OFFSET(BOUNDSCHECKINT), _asm_bounds_check_fault, flags);
    i86_idt_install_ir(IDT32_EXCEPTIONS_INT_OFFSET(INVALIDOPCODEINT), _asm_invalid_opcode_fault, flags);
    i86_idt_install_ir(IDT32_EXCEPTIONS_INT_OFFSET(DEVAVAILIBILITYINT), _asm_device_availibility_fault, flags);
    i86_idt_install_ir(IDT32_EXCEPTIONS_INT_OFFSET(DOUBLEFAULTINT), _asm_double_fault_abort, flags);
    i86_idt_install_ir(IDT32_EXCEPTIONS_INT_OFFSET(INVALIDTSSINT), _asm_invalid_tss_fault, flags);
    i86_idt_install_ir(IDT32_EXCEPTIONS_INT_OFFSET(NOSEGMENTINT), _asm_no_segment_fault, flags);
    i86_idt_install_ir(IDT32_EXCEPTIONS_INT_OFFSET(STACKINT), _asm_stack_fault, flags);
    i86_idt_install_ir(IDT32_EXCEPTIONS_INT_OFFSET(GENPROTECTINT), _asm_general_protection_fault, flags);
    i86_idt_install_ir(IDT32_EXCEPTIONS_INT_OFFSET(PAGEINT), _asm_page_fault, flags);
    i86_idt_install_ir(IDT32_EXCEPTIONS_INT_OFFSET(FPUINT), _asm_fpu_fault, flags);
    i86_idt_install_ir(IDT32_EXCEPTIONS_INT_OFFSET(ALIGNCHECKINT), _asm_alignment_check_fault, flags);
    i86_idt_install_ir(IDT32_EXCEPTIONS_INT_OFFSET(MACHINECHECKINT), _asm_machine_check_abort, flags);
    i86_idt_install_ir(IDT32_EXCEPTIONS_INT_OFFSET(SIMDFPUINT), _asm_simd_fpu_fault, flags);
}
