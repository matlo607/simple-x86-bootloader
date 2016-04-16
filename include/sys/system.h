#ifndef _SYSTEM_H
#define _SYSTEM_H

extern void hang_forever(void);

extern void reboot(void);

extern void panic(const char *format, ...);

#endif
