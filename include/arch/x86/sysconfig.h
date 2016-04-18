#ifndef _SYS_CONFIG_H
#define _SYS_CONFIG_H

#include <arch/x86/checkCCversion.h>

/* Heap */
// Address of heap's beginning (defined in the linker script)
extern unsigned int heap_addr_begin;
#define HEAP_BEGINNING_get_addr() (&heap_addr_begin)

#endif
