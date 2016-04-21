    .code32
    .section .text

    /*************/
    /* Arguments */
    /*************/
    .set saved_ebp, 0
    .set PM_return_addr, saved_ebp + 4
    .set RM_return_addr, saved_ebp + 2
    /* uint8_t intno */
    .set PM_intno, PM_return_addr + 4
    .set RM_intno, RM_return_addr + 2
    /* const x86_regs_t* regs_in */
    .set PM_regs_in, PM_intno + 4
    .set RM_regs_in, RM_intno + 4
    /* x86_regs_t* regs_out */
    .set PM_regs_out, PM_regs_in + 4
    .set RM_regs_out, RM_regs_in + 4
    /* size */
    .set size_args, PM_regs_out + 4 - PM_intno
    .set PM_size_stack_cntxt, PM_regs_out + 4 - saved_ebp
    .set RM_size_stack_cntxt, RM_regs_out + 4 - saved_ebp

    .set x86_regs_t, 0
    .set x86_regs_t_CS, x86_regs_t
    .set x86_regs_t_DS, x86_regs_t_CS + 2
    .set x86_regs_t_ES, x86_regs_t_DS + 2
    .set x86_regs_t_FS, x86_regs_t_ES + 2
    .set x86_regs_t_GS, x86_regs_t_FS + 2
    .set x86_regs_t_SS, x86_regs_t_GS + 2
    .set x86_regs_t_BP, x86_regs_t_SS + 2
    .set x86_regs_t_SP, x86_regs_t_BP + 4
    .set x86_regs_t_A, x86_regs_t_SP + 4
    .set x86_regs_t_B, x86_regs_t_A + 4
    .set x86_regs_t_C, x86_regs_t_B + 4
    .set x86_regs_t_D, x86_regs_t_C + 4
    .set x86_regs_t_eflags, x86_regs_t_D + 4
    .set x86_regs_t_DI, x86_regs_t_eflags + 4
    .set x86_regs_t_SI, x86_regs_t_DI + 4
    .set size_x86_regs_t, x86_regs_t_SI

    .set bp_stack_offset_post_bioscall, 4 + 4 + 4 + 4 + 4 + 2 + 2 + 4 + 4 + 2

    /***************/
    /* Definitions */
    /***************/
.extern cntxt_p_mode_32bit
    .set PM_32bit_cntxt_ptr_gdt, cntxt_p_mode_32bit
    .set PM_32bit_cntxt_regs, cntxt_p_mode_32bit + 6
    .set size_PM_32bit_cntxt, 50

.extern cntxt_p_mode_16bit
    .set PM_16bit_cntxt_ptr_gdt, cntxt_p_mode_16bit
    .set PM_16bit_cntxt_regs, cntxt_p_mode_16bit + 6
    .set size_PM_16bit_cntxt, 50

#define RM_CODE_SEGMENT         0x0000
#define PM_16bit_CODE_SEGMENT   0x8

.extern bioscall

    .global real_mode_int
real_mode_int:
    pushl %ebp
    movl %esp, %ebp

    pushl %edi
    movl $PM_32bit_cntxt_regs, %edi
    movw %cs, x86_regs_t_CS(%edi)
    movw %ds, x86_regs_t_DS(%edi)
    movw %es, x86_regs_t_ES(%edi)
    movw %fs, x86_regs_t_FS(%edi)
    movw %gs, x86_regs_t_GS(%edi)
    movl %eax, x86_regs_t_A(%edi)
    movl %ebx, x86_regs_t_B(%edi)
    movl %ecx, x86_regs_t_C(%edi)
    movl %edx, x86_regs_t_D(%edi)
    movl %ebp, x86_regs_t_BP(%edi)
    movl %esp, x86_regs_t_SP(%edi)
    movl %esi, x86_regs_t_SI(%edi)
    popl %eax
    movl %eax, x86_regs_t_DI(%edi)
    sgdt (PM_32bit_cntxt_ptr_gdt)

_rebase_code:
    /* nothing to do here */

_rebase_data:
    /* copy arguments in real mode data segment */
    /* Get the second argument : pointer on x86_regs_t IN */
    movl PM_regs_in(%ebp), %esi
    /* Compute the first address of the real mode data segment %ds:0000 */
    xorl %eax, %eax
    movw x86_regs_t_DS(%esi), %ax
    shl $4, %eax
    addl $4, %eax /* do not point on 0x0000 (null pointer) */
    movl %eax, %edx /* keep a back up for further use */
    movl %eax, %edi
    /* Compute the end of the value pointed by second argument */
    movl $size_x86_regs_t, %ecx
    addl %eax, %ecx
    /* Copy the IN value pointed by the second argument */
    mov %esi, %ebx
1:
    movl (%ebx), %eax
    movl %eax, (%edi)
    leal 4(%edi), %edi
    leal 4(%ebx), %ebx
    cmp %edi, %ecx
    jg 1b

    /* Compute the end of the value pointed by the third argument */
    addl $size_x86_regs_t, %ecx
    /* Zeroes the area of the OUT value pointed by the third argument */
2:
    movl $0, (%edi)
    leal 4(%edi), %edi
    cmp %edi, %ecx
    jg 2b

    /* Compute the value of the real mode stack pointer */
    movl PM_regs_in(%ebp), %esi
    xorl %eax, %eax
    xorl %ebx, %ebx
    movw x86_regs_t_SS(%esi), %ax
    movw x86_regs_t_SP(%esi), %bx
    shl $4, %eax
    addl %eax, %ebx
    
    /* Allocate space for the arguments, the return address, and the saved %ebp (random value) */
    leal -RM_size_stack_cntxt(%ebx), %edi
    /* Copy args to real mode stack */
    movl %edx, RM_regs_in(%edi)
    leal size_x86_regs_t(%edx), %edx 
    movl %edx, RM_regs_out(%edi)
    movl PM_intno(%ebp), %eax
    movl %eax, RM_intno(%edi)

    /* compute the new value of the real mode stack pointer */
    movl RM_regs_in(%edi), %ebx
    leal 4(%edi), %eax
    movl %eax, x86_regs_t_SP(%ebx)

    movl $0x2000, %ebx
    xorl %esp, %esp
    movw %ax, %sp

    movl %cr0, %eax
    andl $~0x1, %eax
    movl %eax, %cr0
    /*ljmpl $RM_CODE_SEGMENT, $_real_mode*/
