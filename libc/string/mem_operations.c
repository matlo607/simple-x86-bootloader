#include <string.h>
#include <assert.h>

void memset(void* mem, uint8_t c, size_t n)
{
    assert(mem != NULL);

    for (size_t i = 0; i < n; ++i) {
        ((uint8_t*) mem)[i] = c;
    }
}

void memcpy(void *dest, const void *src, size_t n)
{
    assert(dest != NULL);
    assert(src != NULL);

    for (size_t i = 0; i < n; ++i) {
        ((uint8_t*) dest)[i] = ((uint8_t*) src)[i];
    }
}
