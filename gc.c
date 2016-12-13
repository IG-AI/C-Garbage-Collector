#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#include "gc.h"

typedef void * page_t;

struct heap{
  void * memory;
  size_t size;
  bool unsafe_stack;
  float gc_threshold;
  page_t pages;
};



heap_t *
h_init(size_t bytes, bool unsafe_stack, float gc_threshold)
{
  assert(bytes >= 2048);
  heap_t *heap = malloc(sizeof(heap_t) );
  void *memory = malloc(bytes);
  int page_size = 2048;
  int number_of_pages = (bytes / page_size);
  page_t pages = malloc(sizeof(page_t) * number_of_pages);

  page_t ptr = pages;
  for (int i = 0; i < number_of_pages; i++) {
    ptr = memory + i * page_size;
    ptr++;
  }
  
  *heap = ( (heap_t) {memory, bytes, unsafe_stack, gc_threshold, pages} );
  return heap;
}

void *
get_pages(heap_t *h){
  return h->pages;

}

void
h_delete(heap_t *h)
{
  free(h->memory);
  free(h->pages);
  free(h);
}


void 
h_delete_dbg(heap_t *h, void *dbg_value)
{
  return; 
}


void *
h_alloc_struct(heap_t *h, char *layout)
{
  return NULL; 
}


void *
h_alloc_data(heap_t *h, size_t bytes)
{
  return NULL; 
}


size_t 
h_gc(heap_t *h)
{
  return 0; 
}


size_t 
h_gc_dbg(heap_t *h, bool unsafe_stack)
{
  return 0; 
}


size_t h_avail(heap_t *h)
{
  return 0; 
}


size_t h_used(heap_t *h)
{
  return 0; 
}


size_t h_size(heap_t *h)
{
  return 0; 
}
