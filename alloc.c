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

struct obj_metadata *heapStart = NULL;

struct obj_metadata *find_block(size_t size) {
  struct obj_metadata *curr = heapStart;
  while(curr) {
    if(curr->is_free && curr->size <= size) {
      curr->is_free = 0;
      return curr;
    }
    curr = curr->next;
  }
  return NULL;
}


void *mymalloc(size_t size) {
    struct obj_metadata *start = heapStart;
    
    if(size == 0) {
      // go through the heap and find the next available spot of any size
      struct obj_metadata *curr = start;
      while(curr->next) {
        if(curr->is_free) {
          return curr;
        }
      }
    } else {
      int requiredSize = size + (8 - (size % 8)); // 8 bit aligned
      struct obj_metadata *spot = find_block(requiredSize);
      if(spot) {
        spot->is_free = 0;
        return (void *)(spot + 1);
      }
      // if no fit found, create a new block to add to heap
      struct obj_metadata *newBlock;
      int blockSize = requiredSize + sizeof newBlock;
      void *newAddr = sbrk(blockSize);
      newBlock->size = blockSize;
      newBlock->next = NULL;
      newBlock->prev = NULL;
      newBlock->is_free = 0;
      if(!heapStart) {
        heapStart = newBlock;
      }
      // updating end of list to point to 
      struct obj_metadata *curr = start;
      while(curr->next) {
        curr = curr->next;
      }
      curr->next = newBlock;
      return (void *)(newBlock + 1);
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
