#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

struct obj_metadata {
  size_t size;
  struct obj_metadata *next;
  struct obj_metadata *prev;
  int is_free;
};

void *heapStart;

void *mymalloc(size_t size) {
    struct obj_metadata *curr = heapStart;
    if(size == 0) {
      // go through the heap and find the next available spot of any size
      while(curr->next) {
        if(curr->is_free) {
          return curr;
        }
      }
    } else {
      // go through heap and look for first fitting spot
      int requiredSize = size + (8 - (size % 8)); // 8 bit aligned
      while(curr->next) {
        if(curr->is_free && curr->size <= (size_t)requiredSize) {
          curr->is_free = 0;
          return curr;
        }
        curr = curr->next;
      }
      
      // if no fit found, create a new block to add to heap
      struct obj_metadata newBlock;
      void *currentBreak = sbrk(0);                           // get current break
      int blockSize = requiredSize + sizeof newBlock;        // 
      void *newAddr = sbrk(blockSize);                        // allocate appropriately sized block
      curr->next = newAddr;                                   // pointing the previous end of the heap to the new block
      newBlock.size = blockSize;
      newBlock.next = NULL;
      newBlock.prev = curr;
      newBlock.is_free = 0;
      return newAddr;
    }
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
