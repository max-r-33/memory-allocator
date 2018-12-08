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

struct obj_metadata *heapStart  = NULL;
struct obj_metadata *heapEnd    = NULL;
struct obj_metadata *freeBlocks = NULL;
int    heapSize                 = 0;

void print_memory(struct obj_metadata *heap) {
  struct obj_metadata *curr = heap;
  printf("MEMORY:\n");
  while(curr) {
    printf("addr: %p\n", (void *)curr);
    printf("size: %li\n", curr->size);
    printf("is_free: %i\n", curr->is_free);
    printf("prev: %p\n", curr->prev);
    printf("next: %p\n", curr->next);
    printf("---\n");
    curr = curr->next;
  }
}

void print_block(struct obj_metadata *block) {
  printf("addr: %p\n", (void *)block);
  printf("size: %li\n", block->size);
  printf("is_free: %i\n", block->is_free);
  printf("prev: %p\n", block->prev);
  printf("next: %p\n", block->next);
  printf("---\n");
}

void memory_stats() {
  struct obj_metadata *curr = heapStart;
  int count = 0;
  printf("curr: %p and curr->next %p", curr, curr->next);
  while(curr->next) {
    count++;
    curr = curr->next;
  }
}

struct obj_metadata *find_block(size_t size) {
  struct obj_metadata *curr = heapStart;
  while(curr && curr->next) {
    // if we need to split
    if(curr->is_free && (curr->size - (size_t)(sizeof(struct obj_metadata))) >= size) {
      size_t aligned_size = size + (8 - (8 % size));
      if(aligned_size < curr->size) {
        void *new_addr = (void *)(curr + aligned_size + 1);
        ((struct obj_metadata *)new_addr)->size =  (size_t)(curr->size - aligned_size);
        ((struct obj_metadata *)new_addr)->is_free = 1;
        ((struct obj_metadata *)new_addr)->next = curr->next;
        ((struct obj_metadata *)new_addr)->prev = curr;
        curr->next = new_addr; //(void *)curr + aligned_size;
        curr->next->next->prev = new_addr;
        curr->size = aligned_size;
        curr->is_free = 0;
        return curr;
      }
    }
    curr = curr->next;
  }
  return NULL;
}

void *mymalloc(size_t size) {
    struct obj_metadata *start = heapStart;
    if(size <= 0) {
      // go through the heap and find the next available spot of any size
      struct obj_metadata *curr = start;
      while(curr->next) {
        if(curr->is_free) {
          return curr;
        }
        curr = curr->next;
      }
      return sbrk(0);
    }

    size_t requiredSize = size + (8 - (size % 8)); // 8 bit aligned
    struct obj_metadata *spot = freeBlocks ? freeBlocks : find_block(requiredSize);
    if(!spot) {
      // if no fit found, create a new block to add to heap
      size_t blockSize = requiredSize + sizeof(struct obj_metadata);
      spot = sbrk(blockSize);
      spot->size = blockSize;
      spot->next = NULL;
      spot->prev = NULL;
      spot->is_free = 0;
      if(!heapStart) {
        heapStart = spot;
      } else if(heapStart && !heapStart->next) {
        heapStart->next = spot;
      }

      if(heapEnd) {
        heapEnd->next = (void *)spot;
        spot->prev = heapEnd;
      }
      heapSize++;
      heapEnd = spot;
    }
    return (void *)(spot + 1);
}

void *mycalloc(size_t nmemb, size_t size) {
    size_t memsize = nmemb * size; // finding full size of memory to alloc
    if(memsize) {
      void *ptr = mymalloc(memsize); // alloc with mymalloc
      memset(ptr, 0, memsize); // zeroing out newly allocated mem
      return ptr ? ptr : NULL; // if myalloc worked, return ptr, otherwise NULL
    }
    return NULL;
}

void myfree(void *ptr) {
  struct obj_metadata *curr = heapStart;
  struct obj_metadata *ptr_meta = (struct obj_metadata *)ptr - 1;

  while(curr->next) {
    if(curr == ptr_meta) {
      curr->is_free = 1;
      return;
    }
    curr = curr->next;
  }

  curr = heapStart;
  while(curr->next) {
    if(curr->is_free && curr->next->is_free) {
      curr->size += curr->next->size;
      curr->next = curr->next->next;
      heapSize--;
      if(curr->next) {
        curr->next->prev = curr;
      }
    } else {
      curr = curr->next;
    }
  }

  curr = heapStart;
  while(curr->next) {
    if(curr->is_free) {
      struct obj_metadata *currFree = freeBlocks;
      if(currFree) {
        while(currFree->next) {
          currFree = currFree->next;
        }
        currFree->next = curr;
        currFree->next->prev = freeBlocks;
      } else {
        currFree = curr;
        currFree->next = NULL;
        currFree->prev = NULL;
      }
    }
    curr = curr->next;
  }
  print_memory(freeBlocks);

  if(heapEnd->is_free) {
    heapEnd = heapEnd->prev;
    heapEnd->next = NULL;
    sbrk(-1 * heapEnd->size);
  }
}

void *myrealloc(void *ptr, size_t size) {
    if(!ptr || !size) return mymalloc(size);

    struct obj_metadata *curr = (struct obj_metadata *)ptr - 1;
    // if the new size is bigger, do nothing and return the original block
    if(curr->size >= size) return ptr;

    void *newAddr;
    newAddr = mymalloc(size);
    memcpy(newAddr, ptr, curr->size);
    return newAddr;
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
