#ifndef _ARG_H
#define _ARG_H

#include "types.h"

typedef void*   va_list;

#define va_start(args_list, last) \
    args_list = &last + 1

#define va_arg(args_list, type) \
    *( (type*) args_list); args_list += sizeof(type)

#define va_end(args_list) \
    args_list = NULL

#endif
