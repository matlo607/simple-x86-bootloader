#ifndef _I386_TYPE_
#define _I386_TYPE_

/*
 * stdint.h
 * */
typedef unsigned char   uint8_t;
typedef unsigned short  uint16_t;
typedef unsigned int    uint32_t;

typedef signed char     int8_t;
typedef signed short    int16_t;
typedef signed int      int32_t;

/*
 * stdbool.h
 * */
typedef uint8_t     bool;
#define true        1
#define false       0

/*
 * stddef.h
 * */
typedef uint32_t     size_t;
typedef int32_t      ssize_t;

#define EXIT_SUCCESS 0
#define EXIT_FAILURE -1

#define NULL    0

#endif
