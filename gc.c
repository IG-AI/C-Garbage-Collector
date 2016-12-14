#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "gc.h"

//typedef void * page_t;

struct heap{
  void * memory;
  size_t size;
  bool unsafe_stack;
  float gc_threshold;
  void * pages[];
};


heap_t *
h_init(size_t bytes, bool unsafe_stack, float gc_threshold)
{
  assert(bytes >= 2048);
  float c;
  int page_size = 2048;
  int number_of_pages = (bytes / page_size);
  
  if (number_of_pages % 2 == 0) {
    c = 1.5;
  }
  else {
    c = 0.5;
  }

  heap_t *heap = malloc((sizeof(void*) + sizeof(size_t) + sizeof(bool) + sizeof(float)) + (8 * (number_of_pages - c)));

  void *memory = malloc(bytes);
  
  for (int i = 0; i < number_of_pages; i++) {
    heap->pages[i] = memory + (i * page_size);
  }
  
  *heap = ( (heap_t) {memory, bytes, unsafe_stack, gc_threshold} );
  return heap;
}


void *
get_pages(heap_t *h, int i) 
{
  return h->pages[i];
}


void *
get_memory(heap_t *h) 
{
  return h->memory;
}


void
h_delete(heap_t *h)
{
  free(h->memory);
  free(h);
}


void 
h_delete_dbg(heap_t *h, void *dbg_value)
{
  return; 
}

int
de_code(char* layout)
{
  int bytes = 0;
  while(*layout != '\0'){
    if(*layout == 'i'){
      bytes += sizeof(int);
    }
    else if(*layout == '*'){
      bytes += sizeof(void *);
        }
    layout++;
  }
  return bytes;
}


void *
h_alloc_struct(heap_t *h, char *layout)
{
  int bytes = de_code(layout);
  void *ptr = get_pages(h, 0);
  h->pages[0] = h->pages[0] + bytes;
  return ptr; 
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
