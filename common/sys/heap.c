#include "sys/heap.h"

#include <sysconfig.h>
#include <stdio.h>

heap_node_t* heap_list = NULL;

void heap_init(void)
{
    heap_list = (heap_node_t*) HEAP_BEGINNING_get_addr();

    heap_list->next = NULL;
    heap_list->used = false;
    heap_list->size = HEAP_MAX_SIZE - sizeof(heap_node_t);

#ifdef DEBUG_HEAP
    debug_printf("HEAP: initialization start=0x%x, size=%u\r\n", heap_list, heap_list->size);
#endif
}

/*
 * DEBUG functions
 * */
#ifdef DEBUG_HEAP

void debug_print_node(const heap_node_t* node)
{
    debug_printf("HEAP: \tnode = 0x%x (next = 0x%x, mem = 0x%x, used = %b, size = %u)\r\n",
            node,
            node + 1,
            node->next,
            node->used,
            node->size);
}

#endif
