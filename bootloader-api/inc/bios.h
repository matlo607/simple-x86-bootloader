#ifndef _BIOS_H
#define _BIOS_H

#include "x86_regs.h"

/*
 * \brief Perform a bios interrupt.
 * \param[in] intno    : interrupt number ("int 0x__")
 * \param[in] regs_in  : pointer on a x86_regs_t data structure containing initialized
 *                       by the user. It is used to set the registers correctly before
 *                       performing the bios interrupt.
 * \param[in] regs_out : pointer on a x86_regs_t data structure that will be modified
 *                       with the  post-call state of registers. The user has to use
 *                       this structure to get the results from the bios call.
 *
 * \return post-call state of the eflags register
 * */
extern uint32_t bioscall(uint8_t intno, const x86_regs_t* regs_in, x86_regs_t* regs_out);

#endif
