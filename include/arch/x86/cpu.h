#ifndef _CPU_H
#define _CPU_H

#include <stdint.h>

void i86_cpu_init(void);
void i86_cpu_shutdown(void);

/**************
 * Interrupts *
 **************/

// enable external interrupts
static inline void sti()
{
    __asm__ __volatile__ ( "sti" );
}

// disable external interrupts
static inline void cli()
{
    __asm__ __volatile__ ( "cli" );
}

// halt the cpu until the next external interrupt is fired
static inline void hlt()
{
    __asm__ __volatile__ ( "hlt" );
}

/*************
 * Registers *
 *************/

#define CR0_BIT0_PROTECTED_MODE (1)
#define CR0_BIT31_PAGING        (1 << 30)

static inline uint32_t gcr0(void)
{
    uint32_t val;
    __asm__ __volatile__ ( "movl %%cr0, %0" : "=r" (val) );
    return val;
}

static inline void scr0(uint32_t val)
{
    __asm__ __volatile__ ( "movl %0, %%cr0;" : : "r" (val));
}

static inline void lgdt(void* ptr)
{
    __asm__ __volatile__ ( "lgdt (%0)" : : "r" (ptr) );
}

static inline void sgdt(void* ptr)
{
    __asm__ __volatile__ ( "sgdt (%0)" : : "r" (ptr) );
}

static inline void lidt(void* ptr)
{
    __asm__ __volatile__ ( "lidt (%0)" : : "r" (ptr) );
}

static inline void sidt(void* ptr)
{
    __asm__ __volatile__ ( "sidt (%0)" : : "r" (ptr) );
}


#define reg_set_data_segment(ADDR) \
    __asm__ __volatile__ ( "movw %w0, %%ds;" \
                           : \
                           : "r" (ADDR) )

#define reg_get_data_segment(ptr_val_ds) \
    __asm__ __volatile__ ( "movw %%ds, %[ptr];" \
                           : [ptr] "=r" (*ptr_val_ds) \
                           : \
                           : )

#define reg_set_ES_segment(ADDR) \
    __asm__ __volatile__ ( "movw %w0, %%es;" \
                           : \
                           : "r" (ADDR) )

#define reg_set_FS_segment(ADDR) \
    __asm__ __volatile__ ( "movw %w0, %%es;" \
                           : \
                           : "r" (ADDR) )

#define reg_set_GS_segment(ADDR) \
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


/************
 * CPU info *
 ************/

#define CPUID_GETVENDOR   0x0
#define CPUID_GETFEATURES 0x1

enum {
    CPUID_FEAT_ECX_SSE3    = 1 << 0,
    CPUID_FEAT_ECX_PCLMUL  = 1 << 1,
    CPUID_FEAT_ECX_DTES64  = 1 << 2,
    CPUID_FEAT_ECX_MONITOR = 1 << 3,
    CPUID_FEAT_ECX_DS_CPL  = 1 << 4,
    CPUID_FEAT_ECX_VMX     = 1 << 5,
    CPUID_FEAT_ECX_SMX     = 1 << 6,
    CPUID_FEAT_ECX_EST     = 1 << 7,
    CPUID_FEAT_ECX_TM2     = 1 << 8,
    CPUID_FEAT_ECX_SSSE3   = 1 << 9,
    CPUID_FEAT_ECX_CID     = 1 << 10,
    CPUID_FEAT_ECX_FMA     = 1 << 12,
    CPUID_FEAT_ECX_CX16    = 1 << 13,
    CPUID_FEAT_ECX_ETPRD   = 1 << 14,
    CPUID_FEAT_ECX_PDCM    = 1 << 15,
    CPUID_FEAT_ECX_DCA     = 1 << 18,
    CPUID_FEAT_ECX_SSE4_1  = 1 << 19,
    CPUID_FEAT_ECX_SSE4_2  = 1 << 20,
    CPUID_FEAT_ECX_x2APIC  = 1 << 21,
    CPUID_FEAT_ECX_MOVBE   = 1 << 22,
    CPUID_FEAT_ECX_POPCNT  = 1 << 23,
    CPUID_FEAT_ECX_AES     = 1 << 25,
    CPUID_FEAT_ECX_XSAVE   = 1 << 26,
    CPUID_FEAT_ECX_OSXSAVE = 1 << 27,
    CPUID_FEAT_ECX_AVX     = 1 << 28,

    CPUID_FEAT_EDX_FPU     = 1 << 0,
    CPUID_FEAT_EDX_VME     = 1 << 1,
    CPUID_FEAT_EDX_DE      = 1 << 2,
    CPUID_FEAT_EDX_PSE     = 1 << 3,
    CPUID_FEAT_EDX_TSC     = 1 << 4,
    CPUID_FEAT_EDX_MSR     = 1 << 5,
    CPUID_FEAT_EDX_PAE     = 1 << 6,
    CPUID_FEAT_EDX_MCE     = 1 << 7,
    CPUID_FEAT_EDX_CX8     = 1 << 8,
    CPUID_FEAT_EDX_APIC    = 1 << 9,
    CPUID_FEAT_EDX_SEP     = 1 << 11,
    CPUID_FEAT_EDX_MTRR    = 1 << 12,
    CPUID_FEAT_EDX_PGE     = 1 << 13,
    CPUID_FEAT_EDX_MCA     = 1 << 14,
    CPUID_FEAT_EDX_CMOV    = 1 << 15,
    CPUID_FEAT_EDX_PAT     = 1 << 16,
    CPUID_FEAT_EDX_PSE36   = 1 << 17,
    CPUID_FEAT_EDX_PSN     = 1 << 18,
    CPUID_FEAT_EDX_CLF     = 1 << 19,
    CPUID_FEAT_EDX_DTES    = 1 << 21,
    CPUID_FEAT_EDX_ACPI    = 1 << 22,
    CPUID_FEAT_EDX_MMX     = 1 << 23,
    CPUID_FEAT_EDX_FXSR    = 1 << 24,
    CPUID_FEAT_EDX_SSE     = 1 << 25,
    CPUID_FEAT_EDX_SSE2    = 1 << 26,
    CPUID_FEAT_EDX_SS      = 1 << 27,
    CPUID_FEAT_EDX_HTT     = 1 << 28,
    CPUID_FEAT_EDX_TM1     = 1 << 29,
    CPUID_FEAT_EDX_IA64    = 1 << 30,
    CPUID_FEAT_EDX_PBE     = 1 << 31
};

void i86_cpuid(uint32_t code, uint32_t registers[4]);

#endif
