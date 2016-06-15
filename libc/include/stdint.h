#ifndef _STDINT_H
#define _STDINT_H

typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned long long  uint64_t;

typedef signed char         int8_t;
typedef signed short        int16_t;
typedef signed int          int32_t;
typedef signed long long    int64_t;

typedef int32_t             intptr_t;
typedef uint32_t            uintptr_t;


#define INT8_MIN            (-128)
#define INT16_MIN           (-32768)
#define INT32_MIN           (-2147483647 - 1)
#define INT64_MIN           (-9223372036854775807LL - 1)

#define INT8_MAX            127
#define INT16_MAX           32767
#define INT32_MAX           2147483647
#define INT64_MAX           9223372036854775807LL

#define UINT8_MAX           0xff /* 255U */
#define UINT16_MAX          0xffff /* 65535U */
#define UINT32_MAX          0xffffffff  /* 4294967295U */
#define UINT64_MAX          0xffffffffffffffffULL /* 18446744073709551615ULL */

#endif
