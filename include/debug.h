#ifndef _DEBUG_H
#define _DEBUG_H

#include <stdbool.h>

#ifdef DEBUG
    #define DEBUG_CPU
    #define DEBUG_GDT
    #define DEBUG_IDT
    #define DEBUG_PIC
    //#define DEBUG_SHELL
    //#define DEBUG_HEAP
    #define  DEBUG_HD_DRIVER
#endif

#endif
