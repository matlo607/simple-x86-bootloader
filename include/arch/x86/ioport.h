#ifndef _IOPORT_H
#define _IOPORT_H

#include <stdint.h>

static inline void outb(uint16_t port, uint8_t data)
{
    __asm__ __volatile__ ( "outb %%al, %%dx" : : "a" (data), "d" (port) );
}

static inline void outw(uint16_t port, uint16_t data)
{
    __asm__ __volatile__ ( "outw %%ax, %%dx" : : "a" (data), "d" (port) );
}

static inline void outl(uint16_t port, uint32_t data)
{
    __asm__ __volatile__ ( "outl %%eax, %%dx" : : "a" (data), "d" (port) );
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t val;
    __asm__ __volatile__ ( "inb %%dx, %%al" : "=a" (val) : "d" (port) );
    return val;
}

static inline uint16_t inw(uint16_t port)
{
    uint16_t val;
    __asm__ __volatile__ ( "inw %%dx, %%ax" : "=a" (val) : "d" (port) );
    return val;
}

static inline uint32_t inl(uint16_t port)
{
    uint32_t val;
    __asm__ __volatile__ ( "inl %%dx, %%eax" : "=a" (val) : "d" (port) );
    return val;
}

static inline void outsb(uint16_t port, void *buf, uint32_t len)
{
    __asm__ __volatile__ ("rep outsb %%ds:(%0), %3"
            : "=S" (buf), "=c" (len)
            : "m"(buf), "Nd"(port), "0" (buf), "1" (len));
}

static inline void insb(void *buf, uint16_t port, uint32_t len)
{
    __asm__ __volatile__ ("rep insb %3, %%es:(%0)"
            : "=D" (buf), "=c" (len), "=m"(buf)
            : "Nd"(port), "0" (buf), "1" (len));
}

#endif
