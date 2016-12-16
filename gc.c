#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "gc.h"

struct page{
  void * start;
  void * bump;
  int size;
};

typedef struct page page_t;

struct heap{
  void * memory;
  size_t size;
  bool unsafe_stack;
  float gc_threshold;
  int number_of_pages;
  page_t * pages[];
};


heap_t *
h_init(size_t bytes, bool unsafe_stack, float gc_threshold)
{
  assert(bytes >= 2048);
  int page_size = 2048;
  int number_of_pages = (bytes / page_size);
  

  heap_t *heap = malloc( sizeof(void*)
                         + sizeof(size_t)
                         + sizeof(bool) 
                         + sizeof(float) 
                         + sizeof(int) 
                         + (sizeof(void *) * (number_of_pages) ) );

  void *memory = malloc(bytes + (sizeof(page_t) * number_of_pages) );

  *heap = ( (heap_t) {memory, bytes, unsafe_stack, gc_threshold, number_of_pages} );
  
  void *ptr = memory + bytes;
  for (int i = 0; i < number_of_pages; ++i) {
    //heap->pages[i] = malloc(sizeof(page_t) );
    *(page_t *)ptr = ( (page_t) {memory + (i * page_size), memory + (i * page_size), page_size} );
    heap->pages[i] = ptr;
    ptr += sizeof(page_t);
  }
  

  //printf("\nCreating heap. Size:%lu Page size:%d Number of pages:%d Actual number of pages:%d  \n", 
  //       bytes, page_size, number_of_pages, heap->number_of_pages);
  return heap;
}


void *
get_page_start(page_t *page)
{
  return page->start;
}


void *
get_page_bump(page_t *page)
{
  return page->bump;
}


void
set_page_bump(page_t *page, int bytes)
{
  page->bump += bytes;
}


int
get_page_size(page_t *page)
{
  return page->size;
}


size_t get_page_avail(page_t *page)
{
  return ( (page->start + page->size) - page->bump);
}


size_t get_page_used(page_t *page)
{
  return  page->bump - page->start;
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


size_t
de_code(char* layout)
{
  size_t bytes = 0;
  while(*layout != '\0'){
    if(*layout == 'i'){
      bytes += sizeof(int);
    }
    else if(*layout == 'l'){
      bytes += sizeof(long);
        }
    else if(*layout == 'f'){
      bytes += sizeof(float);
        }
    else if(*layout == 'c'){
      bytes += sizeof(char);
        }
    else if(*layout == 'd'){
      bytes += sizeof(double);
        }
    else if(*layout == '*'){
      bytes += sizeof(void *);
        }
    layout++;
  }
  return bytes;
}


int
get_ptr_page(heap_t *h, void * ptr)
{
  int number_of_pages = h->number_of_pages;
  for (int i = 0; i < number_of_pages; i++) {

    //printf("Varv %d\n", i);
    //printf("%p\n%p\n%p\n", h->pages[i]->start, (h->pages[i]->start + h->pages[i]->size),ptr ); 

    if(ptr >= h->pages[i]->start  && ptr < h->pages[i]->start + h->pages[i]->size){ 
    return i;
    }

  }

return -1;
}


void 
write_pointer_to_heap(void ** allocated_memory, void * ptr_to_write)
{
  *allocated_memory = ptr_to_write;
}


void 
write_int_to_heap(void * allocated_memory, int int_to_write)
{
  *(int *)allocated_memory = int_to_write;
}


void *
h_alloc(heap_t * h, size_t bytes)
{
  if(h_avail(h) < h->gc_threshold){
    //run gc
  }

  int page_nr = 0;
  int page_avil = get_page_avail(h->pages[page_nr]);
  //printf("\nAvailable in page(%d):  %d, to write: %lu \n", page_nr, page_avil, bytes);
  while( (int)bytes > page_avil){
    //asserta att vi inte går över page gränsen
      page_nr += 1;
      page_avil = get_page_avail(h->pages[page_nr]);
      //printf("Available in page(%d):  %d,\n", page_nr, page_avil);
    }

  void *page_bump = get_page_bump(h->pages[page_nr]);
  void * ptr = page_bump;
  set_page_bump(h->pages[page_nr], bytes);
  //printf("Pointer is in page:  %d\n", get_ptr_page(h, ptr));  

  return ptr; 
}


void *
h_alloc_struct(heap_t * h, char * layout)
{
  size_t bytes = de_code(layout);
  return h_alloc(h, bytes);
}


void *
h_alloc_data(heap_t *h, size_t bytes)
{
  return h_alloc(h, bytes);
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
  size_t avail = 0;
  int number_of_pages = h->number_of_pages;
  //printf("Number of pages: %d", number_of_pages);
  for (int i = 0; i < number_of_pages; i++) {
    //printf("\nAvil in page %d: %lu\n", i, get_page_avail(h->pages[i]) );
    avail += get_page_avail(h->pages[i]);  
  }
return avail;
}


size_t h_used(heap_t *h)
{
  size_t used = 0;
  int number_of_pages = h->number_of_pages;
  //printf("Number of pages: %d", number_of_pages);
  for (int i = 0; i < number_of_pages; i++) {
    //printf("\nUsed in page %d: %lu\n", i, get_page_used(h->pages[i]) );
    used += get_page_used(h->pages[i]);  
  }
return used;
}


size_t h_size(heap_t *h)
{
  return h->size; 
}
