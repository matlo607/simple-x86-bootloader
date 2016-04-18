#ifndef _CHECK_CC_VERSION_H
#define _CHECK_CC_VERSION_H

#if !defined(__i386__)
    #error "This code needs to be compiled with a i386-elf compiler"
#endif

#endif
