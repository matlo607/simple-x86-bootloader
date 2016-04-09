#ifndef _COMMON
#define _COMMON

#if !defined(__i386__)
#error "This code needs to be compiled with a i386-elf compiler"
#endif

#include "types.h"
#include "config.h"

/*generate 16-bit code*/
__asm__(".code16gcc");

#endif
