#include "k_realloc.h"
#include "kernel.h"
#include <string.h>

struct block_meta {
  size_t size;
  struct block_meta *next;
  int free;
};

struct block_meta *get_block_ptr(void *ptr) {
  return (struct block_meta*)ptr - 1;
}

void * k_realloc_self(void * ptr, size_t size) {
  if (!ptr) { 
    return k_malloc(size);
  }

  struct block_meta* block_ptr = get_block_ptr(ptr);
  if (block_ptr->size >= size) {
    return ptr;
  }

  void *new_ptr;
  new_ptr = k_malloc(size);
  if (!new_ptr) {
    return NULL;
  }
  memcpy(new_ptr, ptr, block_ptr->size);
  k_free(ptr);  
  return new_ptr;
}