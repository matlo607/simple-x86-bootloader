#include "stdio.h"
#include <errno.h>


#include <dev.h>

FILE* fopen(const char* path, const char* mode)
{
    (void) mode;
    FILE* fp = NULL;

    const char* name = path;
    (void) name;

    // TODO

    if (fp == NULL) {
        errno = ENODEV;
    }

    return fp;
}

size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream)
{
    (void) ptr;
    (void) size;
    (void) nmemb;
    (void) stream;

    return 0;
}

size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream)
{
    (void) ptr;
    (void) size;
    (void) nmemb;
    (void) stream;

    return 0;
}

int fclose(FILE* fp)
{
    (void) fp;

    return EXIT_SUCCESS;
}

