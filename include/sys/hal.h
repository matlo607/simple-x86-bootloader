#ifndef _HAL_H
#define _HAL_H

#include <stdint.h>

extern void hal_init(void);
extern void hal_shutdown(void);
extern void hal_geninterrupt(uint8_t intno);

#endif
