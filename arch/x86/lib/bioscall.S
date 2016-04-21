.code16gcc

    .section .text

    .set argv_0, 6
    /* uint8_t intno */
    .set intno, argv_0
    /* const x86_regs_t* regs_in */
    .set regs_in, intno + 4
    /* x86_regs_t* regs_out */
    .set regs_out, regs_in + 4

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

    .set bp_stack_offset_post_bioscall, 4 + 4 + 4 + 4 + 4 + 4 + 2 + 2 + 4 + 4 + 2

    .global bioscall
bioscall:
    pushw %bp
    movw %sp, %bp

    /* save current values of the registers */
    pushl %eax
    pushl %ebx
    pushl %ecx
    pushl %edx
    pushl %edi
    pushl %esi
    pushw %ds
    pushw %es

    /* get the interrupt number */
    movw intno(%bp), %ax
    pushw %ds
    movw %cs, %bx
    movw %bx, %ds /* change %ds to %cs to change source code */
    movb %al, (1f)
    popw %ds

    /* get the pointer on const x86_regs_t* regs_in */
    movw regs_in(%bp), %si

    movw x86_regs_t_DS(%esi), %ax
    movw %ax, %ds
    movw x86_regs_t_ES(%esi), %ax
    movw %ax, %es
    movl x86_regs_t_BP(%esi), %ebp
    movl x86_regs_t_A(%esi), %eax
    movl x86_regs_t_B(%esi), %ebx
    movl x86_regs_t_C(%esi), %ecx
    movl x86_regs_t_D(%esi), %edx
    movl x86_regs_t_DI(%esi), %edi
    movl x86_regs_t_SI(%esi), %esi

    /* call bios interupt */
0:
    /* op code for 'int' */
    .byte 0xcd
1:
    /* interrupt number : default 0 */
    .byte 0x00

    /* save eflags, %edi and %ebp on stack */
    pushf
    pushl %edi
    pushw %bp

    /* restore %ebp so that we can access the arguments */
    movw %sp, %bp
    leaw bp_stack_offset_post_bioscall(%bp), %bp

    /* get the pointer on x86_regs_t* regs_out */
    movl regs_out(%bp), %edi

    movl %eax, x86_regs_t_A(%edi)
    movl %ebx, x86_regs_t_B(%edi)
    movl %ecx, x86_regs_t_C(%edi)
    movl %edx, x86_regs_t_D(%edi)
    /* %ebp */
    xorl %eax,%eax
    popw %ax
    movl %eax, x86_regs_t_BP(%edi)
    /* %edi */
    popl %eax
    movl %eax, x86_regs_t_DI(%edi)
    movl %esi, x86_regs_t_SI(%edi)

    /* eflags */
    popl %eax
    movl %eax, x86_regs_t_eflags(%edi)

    popw %es
    popw %ds
    popl %esi
    popl %edi
    popl %edx
    popl %ecx
    popl %ebx
    popl %eax

    popw %bp
    retl
