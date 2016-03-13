#include "mem.h"

#include "screen.h"
#include "string.h"


typedef struct heap_node_s heap_node_t;
struct heap_node_s {
    heap_node_t* next;
    uint16_t     size;
    bool         used;
};

// Address of heap's beginning (defined in the linker script)
extern heap_node_t heap_addr_begin;

static heap_node_t* heap_list = NULL;


#ifdef DEBUG
void debug_print_node(const heap_node_t* node)
{
    if (node) {
        printf("node = 0x%x :    next = 0x%x, used = %b, size = %u\r\n", node, node->next, node->used, node->size);
    }
}
#endif


/*
 * \brief Allocate a memory area of the given size.
 * \param[in] size : size of the requested memory
 *
 * \return a pointer on the allocated memory
 * */
void* malloc(uint16_t size)
{
    // first time asking memory : initialize the heap structure
    if (heap_list == NULL) {
        heap_list = &heap_addr_begin;

        heap_list->next = NULL;
        heap_list->used = false;
        heap_list->size = HEAP_MAX_SIZE - sizeof(heap_node_t);
    }

    void* allocated_mem = NULL;
    heap_node_t* current_node = heap_list;

    while (current_node != NULL) {
        if ( (!current_node->used) && (current_node->size >= size) ) {

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
                    next_node->size = ((void*)&heap_addr_begin) + HEAP_MAX_SIZE - ((void*) (next_node + 1));
                } else {
                    next_node->size = (current_node->next - next_node - 1) * sizeof(heap_node_t);
                }
                next_node->next = current_node->next;

                current_node->next = next_node;
                current_node->size = size;
            }
            // else just use this node and let the size as it is

            current_node->used = true;
            break;

        } else {
            current_node = current_node->next;
        }
    }

    return allocated_mem;
}

void* calloc(uint16_t nmemb, uint16_t size)
{
    void* mem = malloc(nmemb * size);

    if (mem != NULL) {
        memset(mem, 0, nmemb * size);
    }

    return mem;
}

void* realloc(void *ptr, uint16_t size)
{
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
                free(ptr);
                ptr = malloc(size);
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
                printf("ptr = 0x%x --> double free() pointer\r\n", ptr);
            }

            break;

        } else {
            previous_node = current_node;
            current_node = current_node->next;
        }
    }
}

