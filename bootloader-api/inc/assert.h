#ifndef _ASSERT
#define _ASSERT

#include "common.h"

/*
 * \brief Abort the execution if assertion is false and print a message.
 * \param[in] cond    : assertion
 * \param[in] err_msg : error message to display (optional)
 *
 * */
extern void assert(bool cond, const char* error_msg);

#endif
