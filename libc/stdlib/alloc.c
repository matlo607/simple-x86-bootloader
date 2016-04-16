#include "stdlib.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <sys/system.h>
#include <sys/heap.h>

void* malloc(size_t size)
{
    assert(heap_list != NULL);

    void* allocated_mem = NULL;
    heap_node_t* current_node = heap_list;

#ifdef DEBUG_HEAP
    debug_printf("HEAP: look for a memory area with at least %u bytes available\r\n", size);
#endif

    while (current_node != NULL) {

#ifdef DEBUG_HEAP
        debug_print_node(current_node);
#endif

        if ( (!current_node->used) && (current_node->size >= size) ) {

#ifdef DEBUG_HEAP
            debug_printf("HEAP: available memory at node 0x%x\r\n", current_node);
#endif

            // set allocated_mem without forgetting the offset (size of a heap_list element)
            allocated_mem = current_node + 1;

            // check if the free area is able to make fit the required memory and also,
            // another node and at least 1 byte (otherwise the node is useless)
            if (current_node->size > size + sizeof(heap_node_t)) {

                // create a new node
                heap_node_t* next_node = (heap_node_t*) (allocated_mem + size);
                next_node->used = false;

                // if current_node->next == NULL, it means that we reached the last node of the list
                if (current_node->next == NULL) {
                    next_node->size = ((void*) heap_list) + HEAP_MAX_SIZE - ((void*) (next_node + 1));
                } else {
                    next_node->size = (current_node->next - next_node - 1) * sizeof(heap_node_t);
                }
                next_node->next = current_node->next;

                current_node->next = next_node;
                current_node->size = size;

#ifdef DEBUG_HEAP
                debug_printf("HEAP: create a new node after the newly allocated memory\r\n");
                debug_print_node(next_node);
#endif
            }
            // else just use this node and let the size as it is

            current_node->used = true;
#ifdef DEBUG_HEAP
            debug_printf("HEAP: update the current node\r\n");
            debug_print_node(current_node);
#endif
            break;

        } else {
            current_node = current_node->next;
        }
    }

#ifdef DEBUG_HEAP
    if (allocated_mem == NULL) {
        debug_printf("HEAP: no memory available, malloc() failed\r\n");
    }
#endif

    return allocated_mem;
}

void* calloc(size_t nmemb, size_t size)
{
    void* mem = malloc(nmemb * size);

    if (mem != NULL) {
        memset(mem, 0, nmemb * size);
    }

    return mem;
}

void* realloc(void *ptr, size_t size)
{
    assert(heap_list != NULL);

    heap_node_t* current_node = heap_list;
    void* ptr_mem = NULL;

    // equivalent of malloc()
    if (ptr == NULL) {
        return malloc(size);
    }

    // look for the node corresponding to the given pointer
    while (current_node != NULL) {

        ptr_mem = (void*)(current_node + 1);

        if (ptr_mem == ptr) {

            if (size > current_node->size) {
                // reallocate a more important memory
                ptr = malloc(size);
                // copy all the data into the new memory area
                memcpy(ptr, ptr_mem, current_node->size);

                // free the previously allocated memory area
                free(ptr_mem);
            }
            // else {
            //    // do not change anything
            //}

            return ptr;

        } else {
            current_node = current_node->next;
        }
    }

    // the given pointer was not found, probably corrupted
    return NULL;
}

void free(void *ptr)
{
    assert(heap_list != NULL);

    heap_node_t* current_node = heap_list;
    heap_node_t* previous_node = NULL;

    while (current_node != NULL) {

        void* ptr_mem = (void*)(current_node + 1);

        if (ptr_mem == ptr) {

            if (current_node->used) {
                current_node->used = false;

                // merge free memory area together
                //--------------------------------

                // firstly, merge the newly freed memory with the next one if it is also free
                heap_node_t* next_node = current_node->next;
                if (next_node && (!next_node->used)) {
                    current_node->next = next_node->next;
                    current_node->size = current_node->size + sizeof(heap_node_t) + next_node->size;
                }

                // then merge the newly free memory with the previous one if it is also free
                if (previous_node && (!previous_node->used)) {
                    previous_node->next = current_node->next;
                    previous_node->size = previous_node->size + sizeof(heap_node_t) + current_node->size;
                }

            } else {
                panic("ptr = 0x%x --> double free() pointer", ptr);
            }

            break;

        } else {
            previous_node = current_node;
            current_node = current_node->next;
        }
    }
}

