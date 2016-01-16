#ifndef _SYSTEM
#define _SYSTEM

#include "common.h"

#define interrupts_enable() \
    __asm__ __volatile__ ( "sti" )

#define interrupts_disable() \
    __asm__ __volatile__ ( "cli" )

#define halt_cpu() \
    __asm__ __volatile__ ( "hlt" )

#define reboot() \
    __asm__ __volatile__ ( "int $0x19" )

#ifdef DEBUG

/*
 * \brief Abort the execution if assertion is false and print a message.
 * \param[in] cond    : assertion
 * \param[in] err_msg : error message to display (optional)
 *
 * */
extern void assert(bool cond, const char* error_msg);

#endif

#endif
