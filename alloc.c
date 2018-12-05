#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void *mymalloc(size_t size) {
    if(size == 0) return (void*)((int)sbrk(0) + 1); // if size is 0, return next free addr
    void *currentBreak = sbrk(0); // getting current break
    int newAddr = ((int)currentBreak + size); // finding new address
    int amtToIncrease = 0; // used for 8 bit alignment

    if(newAddr % 8 != 0) { // checking if 8bit aligned
      amtToIncrease = (8-(newAddr % 8));
    }
    
    return sbrk(size + amtToIncrease);
}

void *mycalloc(size_t nmemb, size_t size) {
    size_t memsize = nmemb * size; // finding full size of memory to alloc
    void *ptr = mymalloc(memsize); // alloc with mymalloc
    memset(ptr, 0, memsize); // zeroing out newly allocated mem
    return ptr ? ptr : NULL; // if myalloc worked, return ptr, otherwise NULL
}

void myfree(void *ptr) {
}

void *myrealloc(void *ptr, size_t size) { 
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
