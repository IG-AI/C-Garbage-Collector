#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "gc.h"


#define PAGE_SIZE 2048
#define SMALLEST_ALLOC_SIZE 16


extern char **environ;

struct page{
  void * start;
  void * bump;
  size_t size;
};

typedef struct page page_t;

struct heap{
  void * memory;
  size_t size;
  bool unsafe_stack;
  float gc_threshold;
  size_t number_of_pages;
  page_t * pages[];
};



/**
 *  @brief Creates an array of pages 
 *          
 *  @param  memory pointer to the start of the allocated memory
 *  @param  start_of_pages pointer to the start of the memory area where the pages will be stored
 *  @param  number_of_pages the number of pages to be created
 *  @param  page_size the size of each page
 *  @param  h pointer to the heap struct 
 */
void 
create_pages(void *memory, page_t *start_of_pages, int number_of_pages, size_t page_size, heap_t *h)
{
  for (int i = 0; i < number_of_pages; ++i) {
    unsigned long page_start =  (unsigned long) memory + (i * page_size);
    *start_of_pages = ( (page_t) {(void *) page_start, (void *)page_start, page_size} );
    h->pages[i] = start_of_pages;
    start_of_pages ++;
  }
} 

heap_t *
h_init(size_t bytes, bool unsafe_stack, float gc_threshold)
{
  assert(bytes >= PAGE_SIZE);
  assert(bytes % PAGE_SIZE == 0);
  size_t page_size = PAGE_SIZE;
  size_t number_of_pages = (bytes / page_size);
  

  heap_t *heap = malloc( sizeof(void*)
                         + sizeof(size_t)
                         + sizeof(bool) 
                         + sizeof(float) 
                         + sizeof(int) 
                         + (sizeof(void *) * (number_of_pages) ) );

  if(heap == NULL){
    return NULL;
  }
  
  void *memory = malloc(bytes + (sizeof(page_t) * number_of_pages) );

  if(memory == NULL){
    free(heap);
    return NULL;
  }
  
  *heap = ( (heap_t) {memory, bytes, unsafe_stack, gc_threshold, number_of_pages} );
  
  void *start_of_pages = memory + bytes;
  create_pages(memory, start_of_pages, number_of_pages, page_size, heap);
  
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
  if(h_avail(h) < h->gc_threshold){  //TODO 
    //run gc
  }

  if(bytes < SMALLEST_ALLOC_SIZE){
    bytes = SMALLEST_ALLOC_SIZE; //plus header??
  }

  int page_nr = 0;
  int number_of_pages = h->number_of_pages;
  int page_avail = get_page_avail(h->pages[page_nr]);
  //printf("\nAvailable in page(%d):  %d, to write: %lu \n", page_nr, page_avil, bytes);
  while( (int)bytes > page_avail){
    if(page_nr == number_of_pages - 1){
      return NULL;
    }
    page_nr += 1;
    page_avail = get_page_avail(h->pages[page_nr]);
    //printf("Available in page(%d):  %d,\n", page_nr, page_avil);
  }

  void *page_bump = get_page_bump(h->pages[page_nr]);
  void * ptr_to_write_to = page_bump;
  set_page_bump(h->pages[page_nr], bytes);
  //printf("Pointer is in page:  %d\n", get_ptr_page(h, ptr));  

  return ptr_to_write_to; 
}


void *
h_alloc_struct(heap_t * h, char * layout)
{
  size_t size = get_struct_size(layout);
  //size_t bytes = de_code(layout);
  void * ptr = h_alloc(h, size);
  return create_struct_header(layout, ptr);
}


void *
h_alloc_data(heap_t *h, size_t bytes)
{
  size_t size = get_data_size(bytes);
  void * ptr = h_alloc(h, size);
  return create_data_header(bytes, ptr);
}
/*
//Puts all the pointer found on stack into an array
void pointers_to_array(heap_t *h, void *array[])
{
  void * stack_top = __builtin_frame_address(0);
  void *stack_bottom = (void *)*environ;
  void * heap_start = h->memory;
  void * heap_end = (h->memory + h->size);
  void *pointer = stack_find_next_ptr(&stack_bottom, stack_top, heap_start, heap_end);
  int i = 0;
  while (pointer != NULL)
    {
      array[i] = pointer;
      i += 1;
      pointer = stack_find_next_ptr(&stack_bottom, stack_top, heap_start, heap_end);
    }

}

//TODO
size_t 
h_gc(heap_t *h)
{
  int collected = 0;
  page_t *pages = h->pages; 
  page_t swap = pages[h->number_of_pages - 1];
  int max_num_of_ptrs = h->number_of_pages * (PAGE_SIZE / SMALLEST_ALLOC_SIZE);
  void * collection[max_num_of_ptrs];
  pointers_to_array(h, collection);
  for(int page = 0; page < h->number_of_pages -1; ++page)
    {
      for(int i =0; i< max_num_of_ptrs; ++i)
        {
          if (get_ptr_page(collection[i], h) == page)
            {
              //Flytta data till pagen swap
            }
        }
      //reset the page page = flytta tillbaka bump-ptr
      //flytta tillbaka allt från swap till page (vår allokering på pagen)
    }
  return 0;
}
*/

//??
size_t 
h_gc_dbg(heap_t *h, bool unsafe_stack)
{
  return 0; 
}


size_t 
h_avail(heap_t *h)
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


size_t 
h_used(heap_t *h)
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


size_t
h_size(heap_t *h)
{
  return h->size; 
}
