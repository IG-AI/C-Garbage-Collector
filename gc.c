#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "gc.h"


heap_t *
h_init(size_t bytes, bool unsafe_stack, float gc_threshold)
{
  return 0; 
}

void
h_delete(heap_t *h)
{
  return 0; 
}


void 
h_delete_dbg(heap_t *h, void *dbg_value)
{
  return 0; 
}


void *
h_alloc_struct(heap_t *h, char *layout)
{
  return 0; 
}


void *
h_alloc_data(heap_t *h, size_t bytes)
{
  return 0; 
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


int h_size(heap_t *h)
{
  return 0; 
}
