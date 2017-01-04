/**
 *  @file  gc_hidden.h
 *  @brief Hidden library for gc
 *
 *  @author Daniel Agstrand
 *  @author Henrik Bergendal
 *  @author Adam Inersjo
 *  @author Maria Lindqvist
 *  @author Simon Pellgard
 *  @author Robert Rosborg
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "gc.h"
#include "alloc_map.h"

#ifndef __gc_hidden__
#define __gc_hidden__

extern char **environ;

typedef struct page page_t;
typedef enum page_type page_type_t;

enum page_type
  {
    PASSIVE
    , ACTIVE
    , TRANSITION
  };


struct page
{
  void * start;
  void * bump;
  size_t size;
  page_type_t type;
};


struct heap{
  void *memory;
  alloc_map_t * alloc_map;
  size_t size;
  bool unsafe_stack;
  float gc_threshold;
  size_t number_of_pages;
  page_t *pages[];
};


void *
get_page_start(page_t *page);


void *
get_memory(heap_t *h);

size_t
heap_get_size(heap_t *h);

size_t
heap_get_number_of_pages(heap_t *h);

void *
get_stack_top();

size_t
get_number_of_ptrs_in_stack(heap_t *h, void *original_top);

size_t
get_ptrs_from_stack(heap_t *h, void *original_top, void **array[], size_t array_size);

size_t
get_number_of_active_ptrs(heap_t *h, void *original_top);

size_t
get_active_ptrs(heap_t *h, void *original_top, void **array[], size_t array_size);


#endif
