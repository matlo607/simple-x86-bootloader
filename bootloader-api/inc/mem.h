#ifndef _HEAP_MEM
#define _HEAP_MEM

#include "common.h"

#define HEAP_MAX_SIZE   4096 // 4kB

/*
 * Allocate a memory area in the heap of specified size
 *
 * @return pointer on the newly allocated memory area, otherwise NULL if no memory is available
 * */
extern void* malloc(uint16_t size);

/*
 * Allocate a memory area in the heap of specified size (number of elements * size of each
 * element). The returned area is zeroed.
 *
 * @return pointer on the newly allocated memory area, otherwise NULL if no memory is available
 * */
extern void* calloc(uint16_t nmemb, uint16_t size);

/*
 * Reallocate a memory area referenced by the pointer given in argument to a new memory
 * area of the required size.
 * */
extern void* realloc(void *ptr, uint16_t size);

/*
 * Desallocate the memory referenced by the pointer given in parameter.
 * */
extern void free(void *ptr);

#endif
