#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void *mymalloc(size_t size)
{
    return NULL;
}

void *mycalloc(size_t nmemb, size_t size)
{
    return NULL;
}

void myfree(void *ptr)
{
}

void *myrealloc(void *ptr, size_t size)
{
    return NULL;
}


/*
 * Enable the code below to enable system allocator support for your allocator.
 */
#if 0
void *malloc(size_t size) { return mymalloc(size); }
void *calloc(size_t nmemb, size_t size) { return mycalloc(nmemb, size); }
void *realloc(void *ptr, size_t size) { return myrealloc(ptr, size); }
void free(void *ptr) { myfree(ptr); }
#endif
