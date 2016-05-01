#ifndef _ERRORNO_H
#define _ERRORNO_H

#include <stdint.h>

typedef int8_t status_t;

extern status_t errno;

#define ENOERR  0
#define EINVAL  1 
#define ENODEV  2

#endif
