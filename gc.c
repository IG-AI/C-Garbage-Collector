#define _POSIX_C_SOURCE 200809L

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <malloc.h>

#include "header.h"
#include "stack_search.h"
#include "header_hidden.h"

#include <errno.h>
#include "gc.h"
#include "gc_hidden.h"


#define PAGE_SIZE 2048
#define SMALLEST_ALLOC_SIZE 16





/*============================================================================
 *                             PAGE FUNCTIONS
 *===========================================================================*/

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
    *start_of_pages = ( (page_t) {(void *) page_start, (void *)page_start, page_size, PASSIVE} );
    h->pages[i] = start_of_pages;

    start_of_pages ++;
  }
} 

void *
page_get_start(page_t *page)
{
  return page->start;
}

void *
page_get_end(page_t *page)
{
  return page->start + page->size;
}

void *
page_get_bump(page_t *page)
{
  return page->bump;
}


void
page_move_bump(page_t *page, int bytes)
{
  page->bump += bytes;
}


void
page_reset(page_t * page)
{
  page->bump = page->start;
}

int
page_get_size(page_t *page)
{
  return page->size;
}


size_t
page_get_avail(page_t *page)
{
  return ( (page->start + page->size) - page->bump);
}


size_t 
page_get_used(page_t *page)
{
  return  page->bump - page->start;
}

page_type_t
page_get_type(page_t *page)
{
  return page->type;
}

void
page_set_type(page_t *page, page_type_t type)
{
  page->type = type;
}

/*============================================================================
 *                             HEAP FUNCTIONS
 *===========================================================================*/

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


/*  void *memory; */

/* size_t mem_size = bytes + (sizeof(page_t) * number_of_pages); */

/* //mem_size = 1000*6144; */

/* int res = posix_memalign(&memory, 128, mem_size); */

/* //printf("ptr=%p ret=%d errno=%d ", memory, res, errno); */

/* //printf("\nRes %d", res); */

/* printf("\nmem %lu\n", *(unsigned long *)memory); */
  
void *memory = malloc(bytes + (sizeof(page_t) * number_of_pages) );

//printf("\nmem2 %lu\n", *(unsigned long *)memory2);

  if(memory == NULL){
    free(heap);
    return NULL;
  }
  
  *heap = ( (heap_t) {memory, bytes, unsafe_stack, gc_threshold, number_of_pages} );
  
  void *start_of_pages = memory + bytes;
  create_pages(memory, start_of_pages, number_of_pages, page_size, heap);
  return heap;
}


void *
get_memory(heap_t *h) 
{
  return h->memory;
}

size_t
heap_get_size(heap_t *h)
{
  return h->size;
}

size_t
heap_get_number_of_pages(heap_t *h)
{
  return h->number_of_pages;
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
  void *stack_top = get_stack_top();
  size_t number_of_ptrs = get_number_of_ptrs_in_stack(h, stack_top);
  //printf("\nNumber of pointers: %lu\n", number_of_ptrs);
  void **array[number_of_ptrs];
  get_ptrs_from_stack(h, stack_top, array, number_of_ptrs);
  for(size_t i= 0; i < number_of_ptrs; ++i)
    {
      // printf("\ndel %p\n", array[i]);
      //printf("\nInt: %d\n", *(int *)array[i]);
      *array[i] = dbg_value;
    }
   h_delete(h);
}


#include <setjmp.h>
#define Dump_registers() \
jmp_buf env; \
if (setjmp(env)) abort(); \


/**
 *  @brief Gets the number of pointer found on stack
 *
 *  @param  h the heap
 *  @param  original_bottom the bottom of the stack to search
 */
size_t
get_number_of_ptrs_in_stack(heap_t *h, void *original_top)
{
 
  void *stack_top = original_top;
  void *stack_bottom = (void *)*environ;
  void *heap_start = h->memory;
  void *heap_end = (h->memory + h->size);
  void **pointer = stack_find_next_ptr(&stack_bottom, stack_top, heap_start, heap_end);

  size_t i = 0;
  while (pointer != NULL)
    {
      /* printf("\nPtr %lu found in num%p\n", i,*pointer );
         printf("\nPtr %lu found in num%d\n", i, *(int*)*pointer );*/

      i += 1;
      pointer = stack_find_next_ptr(&stack_bottom, stack_top, heap_start, heap_end);
    }
  return i;
}


//OBS Something strange about the array sizes (gets more space in array from caller function)
/**
 *  @brief Puts all pointer found on stack into an array
 *
 *  @param  h the heap
 *  @param  original_bottom the bottom of the stack to search
 *  @param  array the array to put pointer in
 *  @param  array_size the size of the array
 */
void
get_ptrs_from_stack(heap_t *h, void *original_top, void **array[], size_t array_size)
{
  //printf("Array size: %lu", array_size);
  void *stack_top = original_top;
  void *stack_bottom = (void *)*environ;
  void *heap_start = h->memory;
  void *heap_end = (h->memory + h->size);

  void **pointer = stack_find_next_ptr(&stack_bottom, stack_top, heap_start, heap_end);

  size_t i = 0;
  while (pointer != NULL)
    {
      /*  printf("\nPtr %lu found in ptrsfrom %p\n", i,*pointer );
          printf("\nPtr %lu found in ptrsfrom %d\n", i, *(int *)*pointer );*/

      assert(i < array_size && "There are more ptrs in stack than we thought!");
      array[i] = *pointer;

      i += 1;
      pointer = stack_find_next_ptr(&stack_bottom, stack_top, heap_start, heap_end);
    }
  // printf("\ni = %lu\n", i);
  // assert(i == array_size && "There are less ptrs in stack than we thought!"); This does not work since we get more space in the array
}

int
get_ptr_page(heap_t *h, void * ptr)
{
  int number_of_pages = h->number_of_pages;
  for (int i = 0; i < number_of_pages; ++i) {
    page_t *current = h->pages[i];
    if(ptr >= page_get_start(current) && ptr < page_get_end(current))
      { 
        return i;
      }
  }
  return -1;
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
  int page_avail = page_get_avail(h->pages[page_nr]);
  //printf("\nAvailable in page(%d):  %d, to write: %lu \n", page_nr, page_avil, bytes);
  while( (int)bytes > page_avail){
    if(page_nr == number_of_pages - 1){
      return NULL;
    }
    page_nr += 1;
    page_avail = page_get_avail(h->pages[page_nr]);
    //printf("Available in page(%d):  %d,\n", page_nr, page_avil);
  }

  void *page_bump = page_get_bump(h->pages[page_nr]);
  h->pages[page_nr]->type = ACTIVE;
  void * ptr_to_write_to = page_bump;
  page_move_bump(h->pages[page_nr], bytes);
  //printf("Pointer is in page:  %d\n", get_ptr_page(h, ptr));  

  return ptr_to_write_to; 
}


void *
h_alloc_struct(heap_t * h, char * layout)
{
  assert(*layout != '\0');
  assert(layout != NULL);
  size_t size = get_struct_size(layout);
  assert(size > 0);
  void * ptr = h_alloc(h, size);
  return create_struct_header(layout, ptr);
}


void *
h_alloc_data(heap_t * h, size_t bytes)
{
  assert(bytes > 0);
  size_t size = get_data_size(bytes);
  assert(size <= 2048);
  void * ptr = h_alloc(h, size);
  return create_data_header(bytes, ptr);
}


void *
h_alloc_raw(heap_t * h, void * ptr_to_data)
{
  assert(ptr_to_data != NULL);
  size_t data_size = get_existing_size(ptr_to_data);
  int page_nr = 0;
  int number_of_pages = h->number_of_pages - 1;
  int page_avail = page_get_avail(h->pages[page_nr]);

  while( (int)data_size > page_avail){
    if(page_nr == number_of_pages - 1){
      return NULL;
    }
    page_nr += 1;
    page_avail = page_get_avail(h->pages[page_nr]);
   }

  void *page_bump = page_get_bump(h->pages[page_nr]);
  void * ptr_to_write_to = page_bump;
  page_move_bump(h->pages[page_nr], data_size);

  return memcpy(ptr_to_write_to, ptr_to_data, data_size);
  
 
    

}


void * get_stack_top(){
  return __builtin_frame_address(0);
}

//TODO
size_t 
h_gc(heap_t *h)
{
  size_t used_before_gc = h_used(h);
  //page_t * pages = h->pages; 
  page_t * swap = h->pages[h->number_of_pages - 1];

  void *stack_top = get_stack_top();
  size_t num_ptrs_in_stack = get_number_of_ptrs_in_stack(h, stack_top);
  void ** array_of_found_ptrs[num_ptrs_in_stack];
  get_ptrs_from_stack(h, stack_top,  array_of_found_ptrs, num_ptrs_in_stack);

  for(size_t page_nr = 0; page_nr < h->number_of_pages -1; ++page_nr)
    {
      int  swap_array_num_of_indexes = 0;
      void * swap_array[num_ptrs_in_stack];
      for(size_t i = 0; i < num_ptrs_in_stack; ++i)  //num_ptrs_in_stack gets the wrong number of ptrs - segfault??
        {
          int index_in_swap_page = 0;
          if (get_ptr_page(* array_of_found_ptrs[i], h) == (int) page_nr)  
            {
              //Flytta data till pagen swap
              void * src_ptr = (void *)( (unsigned long) * array_of_found_ptrs[i] - sizeof(void *) ); // use copy_header(obs sizes)
              void * dest_ptr = swap->bump;
              size_t data_size = get_existing_size(* array_of_found_ptrs[i]);
              swap->bump += data_size;
              void * pointer_to_swap_obj = memcpy(dest_ptr, src_ptr, data_size);
              swap_array[index_in_swap_page] = pointer_to_swap_obj;
              ++index_in_swap_page;
              // Forwarding and not copying same data twice
            }
          swap_array_num_of_indexes = index_in_swap_page;
        }
      page_reset(h->pages[page_nr]);
      for(int j = 0; j <= swap_array_num_of_indexes; ++j){
        h_alloc_raw(h, swap_array[j]);
        // Change stack pointer to new location
      }
      page_reset(swap);
      //reset the page page = flytta tillbaka bump-ptr
      //flytta tillbaka allt från swap till page (vår allokering på pagen)
    }
  size_t used_after_gc = h_used(h);
  size_t collected = used_before_gc - used_after_gc;
  return collected;
}


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
    //printf("\nAvil in page %d: %lu\n", i, page_get_avail(h->pages[i]) );
    avail += page_get_avail(h->pages[i]);  
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
    //printf("\nUsed in page %d: %lu\n", i, page_get_used(h->pages[i]) );
    used += page_get_used(h->pages[i]);  
  }
return used;
}


size_t
h_size(heap_t *h)
{
  return h->size; 
}

