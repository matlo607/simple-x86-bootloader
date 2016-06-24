#include "arch/x86/cpu.h"

#include <debug.h>
#include <string.h>
#include <stdio.h>

#include <arch/x86/pic.h>
#include <arch/x86/cpu_mode_switching.h>

cpu_switch_mode_context_t cntxt_p_mode_16bit;
cpu_switch_mode_context_t cntxt_p_mode_32bit;

void i86_cpu_init(void)
{
    // clear the structures
    memset(&cntxt_p_mode_32bit, 0, sizeof(cpu_switch_mode_context_t));
    memset(&cntxt_p_mode_16bit, 0, sizeof(cpu_switch_mode_context_t));

    // set up memory segmentation
    i86_gdt_init();

    // set up interrupts
    i86_idt_init();

#ifdef DEBUG_CPU
    // get CPU's information
    char cpu_vendor[17] = {'\0'};
    uint32_t registers[4] = {0};
    i86_cpuid(CPUID_GETVENDOR, registers);

    char* temp = cpu_vendor;
    memcpy(temp, &registers[1], sizeof(registers[1]));
    temp += sizeof(registers[1]);
    memcpy(temp, &registers[3], sizeof(registers[3]));
    temp += sizeof(registers[3]);
    memcpy(temp, &registers[2], sizeof(registers[2]));

    debug_printf("cpu: vendor: %s\r\n", cpu_vendor);

    memset(registers, 0, sizeof(registers));
    i86_cpuid(CPUID_GETFEATURES, registers);

    debug_printf("cpu: features: ");
    if (registers[3] & CPUID_FEAT_EDX_FPU) {
        printf("FPU ");
    }
    if (registers[3] & CPUID_FEAT_EDX_VME) {
        printf("VME ");
    }
    if (registers[3] & CPUID_FEAT_EDX_DE) {
        printf("DE ");
    }
    if (registers[3] & CPUID_FEAT_EDX_PSE) {
        printf("PSE ");
    }
    if (registers[3] & CPUID_FEAT_EDX_TSC) {
        printf("TSC ");
    }
    if (registers[3] & CPUID_FEAT_EDX_MSR) {
        printf("MSR ");
    }
    if (registers[3] & CPUID_FEAT_EDX_PAE) {
        printf("PAE ");
    }
    if (registers[3] & CPUID_FEAT_EDX_MCE) {
        printf("MCE ");
    }
    if (registers[3] & CPUID_FEAT_EDX_CX8) {
        printf("CX8 ");
    }
    if (registers[3] & CPUID_FEAT_EDX_APIC) {
        printf("APIC ");
    }
    if (registers[3] & CPUID_FEAT_EDX_SEP) {
        printf("SEP ");
    }
    if (registers[3] & CPUID_FEAT_EDX_MTRR) {
        printf("MTRR ");
    }
    if (registers[3] & CPUID_FEAT_EDX_PGE) {
        printf("PGE ");
    }
    if (registers[3] & CPUID_FEAT_EDX_MCA) {
        printf("MCA ");
    }
    if (registers[3] & CPUID_FEAT_EDX_CMOV) {
        printf("CMOV ");
    }
    if (registers[3] & CPUID_FEAT_EDX_PAT) {
        printf("PAT ");
    }
    if (registers[3] & CPUID_FEAT_EDX_PSE36) {
        printf("PSE36 ");
    }
    if (registers[3] & CPUID_FEAT_EDX_PSN) {
        printf("PSN ");
    }
    if (registers[3] & CPUID_FEAT_EDX_CLF) {
        printf("CLF ");
    }
    if (registers[3] & CPUID_FEAT_EDX_DTES) {
        printf("DTES ");
    }
    if (registers[3] & CPUID_FEAT_EDX_ACPI) {
        printf("ACPI ");
    }
    if (registers[3] & CPUID_FEAT_EDX_MMX) {
        printf("MMX ");
    }
    if (registers[3] & CPUID_FEAT_EDX_FXSR) {
        printf("FXSR ");
    }
    if (registers[3] & CPUID_FEAT_EDX_SSE) {
        printf("SSE ");
    }
    if (registers[3] & CPUID_FEAT_EDX_SSE2) {
        printf("SSE2 ");
    }
    if (registers[3] & CPUID_FEAT_EDX_SS) {
        printf("SS ");
    }
    if (registers[3] & CPUID_FEAT_EDX_HTT) {
        printf("HTT ");
    }
    if (registers[3] & CPUID_FEAT_EDX_TM1) {
        printf("TM1 ");
    }
    if (registers[3] & CPUID_FEAT_EDX_IA64) {
        printf("IA64 ");
    }
    if (registers[3] & CPUID_FEAT_EDX_PBE) {
        printf("PBE ");
    }
    if (registers[2] & CPUID_FEAT_ECX_VMX) {
        printf("VMX ");
    }
    // etc ...

    printf("\r\n");
#endif
}

void i86_cpu_shutdown(void)
{

}

void i86_cpuid(uint32_t code, uint32_t registers[4])
{
    __asm__ __volatile__(
            "cpuid;"
            : "=a" (registers[0]),
              "=b" (registers[1]),
              "=c" (registers[2]),
              "=d" (registers[3])
            : "a" (code) );
}
