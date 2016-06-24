/***************
 ** FUCK GCC ! *
 ***************
 *
 * GCC 6.1 cannot handle __attribute__ (interrupt) for x86 architecture but can for others architectures like ARM.
 * https://gcc.gnu.org/onlinedocs/gcc-6.1.0/gcc/x86-Function-Attributes.html#x86-Function-Attributes
 * https://gcc.gnu.org/onlinedocs/gcc-6.1.0/gcc/ARM-Function-Attributes.html#ARM-Function-Attributes
 *
 * The bug 66960 is open (https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66960),
 * I just hope the bug fix will belong to the next release.
 *
 * Example of how it would have been beautiful :
 * void __attribute__ (interrupt) my_isr (void);
 *
 **/

/* Default handlers */
.extern isr_default_int
.extern isr_default_pic1_int
.extern isr_default_pic2_int

/* Exceptions */
.extern divide_by_zero_fault
.extern single_step_trap
.extern nmi_trap
.extern breakpoint_trap
.extern overflow_trap
.extern bounds_check_fault
.extern invalid_opcode_fault
.extern device_availibility_fault
.extern double_fault_abort
.extern invalid_tss_fault
.extern no_segment_fault
.extern stack_fault
.extern general_protection_fault
.extern page_fault
.extern fpu_fault
.extern alignment_check_fault
.extern machine_check_abort
.extern simd_fpu_fault

/* send to PIC EOI signal
 * \param[in] intno : interrupt's number
 */
.extern i86_pic_endofinterrupt

/* Hardware interrupts */
.extern isr_clock_int
.extern isr_keyboard_int

/* Software interrupts */
.extern isr_timer_int
.extern isr_kbd_getinput_int

.section .text

/* default interrupt handler */
.global _asm_default_int
.align  4
.type   _asm_default_int, @function
_asm_default_int:
    pushal
    call isr_default_int
    popal
    iret

/* default PIC1's interrupt handler */
.global _asm_default_pic1_int
.align  4
.type   _asm_default_pic1_int, @function
_asm_default_pic1_int:
    pushal
    call isr_default_pic1_int
    movb $0x20, %al
    outb %al, $0x20
    popal
    iret

/* default PIC2's interrupt handler */
.global _asm_default_pic2_int
.align  4
.type   _asm_default_pic2_int, @function
_asm_default_pic2_int:
    pushal
    call isr_default_pic2_int
    movb $0x20, %al
    outb %al, $0xa0
    popal
    iret

/*********************************
 * hardware interrupts' handlers *
 *********************************/
.global _asm_clock_int
.align  4
.type _asm_clock_int, @function
_asm_clock_int:
    pushal
    call isr_clock_int
    /*pushl $0
      call i86_pic_endofinterrupt
      popl %eax*/
    movb $0x20, %al
    outb %al, $0x20
    popal
    iret

.global _asm_keyboard_int
.align  4
.type _asm_keyboard_int, @function
_asm_keyboard_int:
    pushal
    call isr_keyboard_int
    /*push $0x1
      call i86_pic_endofinterrupt
      popl %eax*/
    movb $0x20, %al
    outb %al, $0x20
    popal
    iret

/********************************
 * Software interrupts' handler *
 ********************************/
.global _asm_timer_int
.align  4
.type _asm_timer_int, @function
_asm_timer_int:
    pushal
    call isr_timer_int
    popal
    iret

.global _asm_kbd_getinput_int
.align  4
.type _asm_kbd_getinput_int, @function
_asm_kbd_getinput_int:
    pushal
    call isr_kbd_getinput_int
    popal
    iret

/**************
 * Exceptions *
 **************/
.global _asm_divide_by_zero_fault
.align  4
.type _asm_divide_by_zero_fault, @function
_asm_divide_by_zero_fault:
    pushal
    call divide_by_zero_fault
    popal
    iret

.global _asm_single_step_trap
.align  4
.type _asm_single_step_trap, @function
_asm_single_step_trap:
    pushal
    call single_step_trap
    popal
    iret

.global _asm_nmi_trap
.align  4
.type _asm_nmi_trap, @function
_asm_nmi_trap:
    pushal
    call nmi_trap
    popal
    iret

.global _asm_breakpoint_trap
.align  4
.type _asm_breakpoint_trap, @function
_asm_breakpoint_trap:
    pushal
    call breakpoint_trap
    popal
    iret

.global _asm_overflow_trap
.align  4
.type _asm_overflow_trap, @function
_asm_overflow_trap:
    pushal
    call overflow_trap
    popal
    iret

.global _asm_bounds_check_fault
.align  4
.type _asm_bounds_check_fault, @function
_asm_bounds_check_fault:
    pushal
    call bounds_check_fault
    popal
    iret

.global _asm_invalid_opcode_fault
.align  4
.type _asm_invalid_opcode_fault, @function
_asm_invalid_opcode_fault:
    pushal
    call invalid_opcode_fault
    popal
    iret

.global _asm_device_availibility_fault
.align  4
.type _asm_device_availibility_fault, @function
_asm_device_availibility_fault:
    pushal
    call device_availibility_fault
    popal
    iret

.global _asm_double_fault_abort
.align  4
.type _asm_double_fault_abort, @function
_asm_double_fault_abort:
    pushal
    call double_fault_abort
    popal
    iret

.global _asm_invalid_tss_fault
.align  4
.type _asm_invalid_tss_fault, @function
_asm_invalid_tss_fault:
    pushal
    call invalid_tss_fault
    popal
    iret

.global _asm_no_segment_fault
.align  4
.type _asm_no_segment_fault, @function
_asm_no_segment_fault:
    pushal
    call no_segment_fault
    popal
    iret

.global _asm_stack_fault
.align  4
.type _asm_stack_fault, @function
_asm_stack_fault:
    pushal
    call stack_fault
    popal
    iret

.global _asm_general_protection_fault
.align  4
.type _asm_general_protection_fault, @function
_asm_general_protection_fault:
    pushal
    call general_protection_fault
    popal
    iret

.global _asm_page_fault
.align  4
.type _asm_page_fault, @function
_asm_page_fault:
    pushal
    call page_fault
    popal
    iret

.global _asm_fpu_fault
.align  4
.type _asm_fpu_fault, @function
_asm_fpu_fault:
    pushal
    call fpu_fault
    popal
    iret

.global _asm_alignment_check_fault
.align  4
.type _asm_alignment_check_fault, @function
_asm_alignment_check_fault:
    pushal
    call alignment_check_fault
    popal
    iret

.global _asm_machine_check_abort
.align  4
.type _asm_machine_check_abort, @function
_asm_machine_check_abort:
    pushal
    call machine_check_abort
    popal
    iret

.global _asm_simd_fpu_fault
.align  4
.type _asm_simd_fpu_fault, @function
_asm_simd_fpu_fault:
    pushal
    call simd_fpu_fault
    popal
    iret
