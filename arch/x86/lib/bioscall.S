.code16gcc

.text

.global bioscall
bioscall:
    pushw %bp
    movw %sp, %bp

    /* save current values of the registers */
    pushl %ebx
    pushl %ecx
    pushl %edx
    pushl %edi
    pushl %esi
    pushw %ds
    pushw %es

    /* get the interrupt number */
    movw 0x06(%bp), %ax
    pushw %ds
    movw %cs, %bx
    movw %bx, %ds
    movb %al, (1f)
    popw %ds

    /* get the pointer on const x86_regs_t* regs_in */
    movw 0x0a(%bp), %si

    movw (%esi), %ax
    movw %ax, %ds
    movw 0x02(%esi), %ax
    movw %ax, %es
    /* movw 0x04(%esi), %fs */
    /* movw 0x06(%esi), %gs */
    movl 0x08(%esi), %ebp
    movl 0x0c(%esi), %eax
    movl 0x10(%esi), %ebx
    movl 0x14(%esi), %ecx
    movl 0x18(%esi), %edx
    /* skip eflags (output value) */
    movl 0x20(%esi), %edi
    movl 0x24(%esi), %esi

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
    leaw 0x22(%bp), %bp

    /* get the pointer on x86_regs_t* regs_out */
    movl 0xe(%bp), %edi

    movl %eax, 0x0c(%edi)
    movl %ebx, 0x10(%edi)
    movl %ecx, 0x14(%edi)
    movl %edx, 0x18(%edi)
    /* %ebp */
    xorl %eax,%eax
    popw %ax
    movl %eax, 0x08(%edi)
    /* %edi */
    popl %eax
    movl %eax, 0x20(%edi)
    movl %esi, 0x24(%edi)

    /* return eflags in %eax, and also write them in regs_out */
    popl %eax
    movl %eax, 0x1c(%edi)

    popw %es
    popw %ds
    popl %esi
    popl %edi
    popl %edx
    popl %ecx
    popl %ebx

    popw %bp
    retl
