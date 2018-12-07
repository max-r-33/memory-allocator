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
struct obj_metadata *heapEnd   = NULL;

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

void print_memory() {
  struct obj_metadata *curr = heapStart;
  printf("MEMORY:\n");
  while(curr) {
    printf("size: %li\n", curr->size);
    printf("is_free: %i\n", curr->is_free);
    printf("---\n");
    curr = curr->next;
  }
}

void coalesce_blocks() {
  struct obj_metadata *curr = heapStart;
  printf("coalesce_blocks\n");
  while(curr) {
    if(curr->is_free && curr->next->is_free) {
      curr->next = curr->next->next;
      curr->size += curr->next->size;
      curr->is_free = 1;
    }
    curr = curr->next;
  }
  // print_memory();
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
        curr = curr->next;
      }
      return sbrk(0);
    }

    size_t requiredSize = size + (8 - (size % 8)); // 8 bit aligned
    struct obj_metadata *spot = find_block(requiredSize);

    if(spot) {
      spot->is_free = 0;
    } else {
      // if no fit found, create a new block to add to heap
      size_t blockSize = requiredSize + sizeof(struct obj_metadata);
      spot = sbrk(blockSize);
      spot->size = size;
      spot->next = NULL;
      spot->prev = NULL;
      spot->is_free = 0;

      if(!heapStart) {
        heapStart = spot;
      }

      if(heapEnd) {
        heapEnd->next = spot;
        spot->prev = heapEnd;
        heapEnd->next = spot;
      }
      heapEnd = spot;
      print_memory();
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

  while(curr) {
    if(curr == ptr) {
      curr->is_free = 1;
      return;
    }
    curr = curr->next;
  }

  curr = heapStart;
  while(curr->next) {
    if(curr->is_free && curr->next->is_free) {
      printf("merging blocks\n");
      curr->next = curr->next->next;
      curr->size += curr->next->size;
      curr->is_free = 1;
    }
    curr = curr->next;
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
