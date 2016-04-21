#ifndef _BIOS_H
#define _BIOS_H

#include <arch/x86/checkCCversion.h>
#include <arch/x86/regs.h>

/*
 * \brief Perform a bios interrupt (only use it in real mode)
 * \param[in] intno    : interrupt number ("int 0x__")
 * \param[in] regs_in  : pointer on a x86_regs_t data structure containing initialized
 *                       by the user. It is used to set the registers correctly before
 *                       performing the bios interrupt.
 * \param[in] regs_out : pointer on a x86_regs_t data structure that will be modified
 *                       with the  post-call state of registers. The user has to use
 *                       this structure to get the results from the bios call.
 *
 * */
extern void bioscall(uint8_t intno, const x86_regs_t* regs_in, x86_regs_t* regs_out);

#ifdef BOOTLOADER_PROTECTED_MODE_ENABLED
/*
 * \brief Perform a bios interrupt (only use it in 32-bit protected mode)
 * \param[in] intno    : interrupt number ("int 0x__")
 * \param[in] regs_in  : pointer on a x86_regs_t data structure containing initialized
 *                       by the user. It is used to set the registers correctly before
 *                       performing the bios interrupt.
 * \param[in] regs_out : pointer on a x86_regs_t data structure that will be modified
 *                       with the  post-call state of registers. The user has to use
 *                       this structure to get the results from the bios call.
 *
 * */
extern void real_mode_int(uint8_t intno, const x86_regs_t* regs_in, x86_regs_t* regs_out);


extern void int32(uint8_t intnum, regs16_t *regs);
#endif

#endif
