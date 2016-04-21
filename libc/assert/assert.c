#include "assert.h"

#include <sys/system.h>

#ifdef DEBUG

void __assert_fail(const char* _assertion,
                   const char* _file,
                   unsigned int _line,
                   const char* _func)
{
    panic("%s:%u: %s: Assertion `%s' failed.", _file, _line, _func, _assertion);
}

#endif
