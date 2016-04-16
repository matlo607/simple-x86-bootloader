#ifndef _SYS_SETUP_H
#define _SYS_SETUP_H

#include <stdbool.h>

extern bool platform_specific_startup(void);

extern bool platform_startup(void);

#endif
