#ifndef _HEAP_H
#define _HEAP_H

#include <stdbool.h>
#include <stddef.h>
#include <debug.h>

#define HEAP_MAX_SIZE        4096 // 4kB

typedef struct heap_node_s heap_node_t;
struct heap_node_s {
    heap_node_t* next;
    size_t       size;
    bool         used;
};

extern heap_node_t* heap_list;

/*
 * Initialize the heap
 * */
extern void heap_init(void);

/*
 * DEBUG functions
 * */
#ifdef DEBUG_HEAP
extern void debug_print_node(const heap_node_t* node);
#endif

#endif
