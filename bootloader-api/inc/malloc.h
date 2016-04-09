#ifndef _MALLOC
#define _MALLOC

#include "common.h"

/*
 * Initialize the heap
 * */
extern void heap_init(void);

/*
 * Allocate a memory area in the heap of specified size
 *
 * @return pointer on the newly allocated memory area, otherwise NULL if no memory is available
 * */
extern void* malloc(size_t size);

/*
 * Allocate a memory area in the heap of specified size (number of elements * size of each
 * element). The returned area is zeroed.
 *
 * @return pointer on the newly allocated memory area, otherwise NULL if no memory is available
 * */
extern void* calloc(size_t nmemb, size_t size);

/*
 * Reallocate a memory area referenced by the pointer given in argument to a new memory
 * area of the required size.
 * */
extern void* realloc(void *ptr, size_t size);

/*
 * Desallocate the memory referenced by the pointer given in parameter.
 * */
extern void free(void *ptr);

#endif
