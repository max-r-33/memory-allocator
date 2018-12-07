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

void print_memory() {
  struct obj_metadata *curr = heapStart;
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
  printf("BLOCK\n");
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
  while(curr) {
    count++;
    curr = curr->next;
  }
  printf("NUMBER OF OBJECTS IN MEM: %i", count);
}

struct obj_metadata *find_block(size_t size) {
  struct obj_metadata *curr = heapStart;
  while(curr) {
    if(curr->is_free && curr->size >= size) {
      printf("FOUND FREE SPOT");
      curr->is_free = 0;
      return curr;
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
    struct obj_metadata *spot = find_block(requiredSize);
    if(spot) {
      print_block(spot);
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
      } else if(heapStart && !heapStart->next) {
        heapStart->next = spot;
      }

      if(heapEnd) {
        heapEnd->next = (void *)spot;
        spot->prev = heapEnd;
      }

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
  printf("MEMORY BEFORE COALESCE\n");
  memory_stats();
  // print_memory();
  // while(curr) {
  //   if(curr->is_free) {
  //     int merged_next = 0;
  //     if(curr->next->is_free) {
  //       printf("merging next and curr\n");
  //       // print_block(curr);
  //       curr->next = curr->next->next;
  //       curr->size += curr->next->size;
  //       // print_block(curr);
  //       merged_next = 1;
  //     }
  //     if(curr->prev && curr->prev->is_free) {
  //       printf("merging prev and curr\n");
  //       curr->prev->next = curr->next;
  //       curr->prev->size += curr->size;
  //     }
  //     curr = merged_next ? curr->next->next : curr;
  //   } else {
  //     curr = curr->next;
  //   }
  // }
  while(curr->next) {
    if(curr->is_free && curr->next->is_free) {
      curr->size += curr->next->size;
      curr->next = curr->next->next;
      if(curr->next) {
        curr->next->prev = curr;
      }
    } else {
      curr = curr->next;
    }
  }

  printf("MEMORY AFTER COALESCE\n");
  memory_stats();
  print_memory();

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
