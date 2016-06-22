#ifndef _ASSERT_H
#define _ASSERT_H

#ifdef DEBUG

void __assert_fail(const char* _assertion,
                   const char* _file,
                   unsigned int _line,
                   const char* _func);

/*
 * \brief Abort the execution if the assertion is false and print details.
 *
 * */
#define assert(assertion) \
    if (!(assertion)) { \
        __assert_fail(#assertion, __FILE__, __LINE__, __func__); \
    }

#define assert_if(cond, assertion) \
    if ((cond) && !(assertion)) {\
        __assert_fail(#assertion, __FILE__, __LINE__, __func__); \
    }

#else

#define assert(assertion) {}
#define assert_if(cond, assertion) {}

#endif

#endif
