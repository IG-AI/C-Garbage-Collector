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
#include "alloc_map.h"

#include <errno.h>
#include "gc.h"
#include "gc_hidden.h"



#define HEADER_SIZE 8
#define WORD_SIZE 8
#define PAGE_SIZE 2048
#define SMALLEST_ALLOC_SIZE 16


#include <setjmp.h>
#define Dump_registers()                        \
  jmp_buf env;                                  \
  if (setjmp(env)) abort();                     \


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
  for (int i = 0; i < number_of_pages; ++i) 
    {
      unsigned long page_start =  (unsigned long) memory + (i * page_size);
      *start_of_pages = ( (page_t) {(void *) page_start, (void *)page_start, page_size, PASSIVE} );
      h->pages[i] = start_of_pages;
      ++start_of_pages;
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
  assert(bytes >= PAGE_SIZE*2);
  if(!(bytes >= PAGE_SIZE*2)) return NULL;
  assert(bytes % PAGE_SIZE == 0);
  if(!(bytes % PAGE_SIZE == 0)) return NULL;
  assert(0 < gc_threshold && gc_threshold <= 1);
  if(!(0 < gc_threshold && gc_threshold <= 1)) return NULL;

  size_t number_of_pages = (bytes / PAGE_SIZE);
  

  /*heap_t *heap = malloc( sizeof(void*)
                         + sizeof(alloc_map_t *)
                         + sizeof(size_t)
                         + sizeof(bool) 
                         + sizeof(float) 
                         + sizeof(size_t) 
                         + (sizeof(void *) * (number_of_pages) ) );

  if(heap == NULL){
    return NULL;
  }

  */
  /*  void *memory; */

  /* size_t mem_size = bytes + (sizeof(page_t) * number_of_pages); */

  /* //mem_size = 1000*6144; */

  /* int res = posix_memalign(&memory, 128, mem_size); */

  /* //printf("ptr=%p ret=%d errno=%d ", memory, res, errno); */

  /* //printf("\nRes %d", res); */

  /* printf("\nmem %lu\n", *(unsigned long *)memory); */
  size_t heap_struct_size = sizeof(void*) + sizeof(alloc_map_t *) + sizeof(size_t)
                            + sizeof(bool)  + sizeof(float)  + sizeof(size_t) 
                            + (sizeof(void *) * (number_of_pages) );
  
  size_t alloc_map_size = alloc_map_mem_size_needed(WORD_SIZE, bytes);
  size_t pages_size = (sizeof(page_t) * number_of_pages);
  void *ptr_to_allocated_space = malloc(heap_struct_size + bytes + alloc_map_size + pages_size);

  //printf("\nmem2 %lu\n", *(unsigned long *)memory2);
  if(ptr_to_allocated_space == NULL)
    {
      return NULL;
    }

  heap_t *heap = ptr_to_allocated_space;
  heap->memory = (void *) ((size_t) ptr_to_allocated_space + heap_struct_size);
  heap->alloc_map = (alloc_map_t *) ( (size_t) heap->memory + bytes);
  heap->size = bytes;
  heap->unsafe_stack = unsafe_stack;
  heap->gc_threshold = gc_threshold;
  heap->number_of_pages = number_of_pages;

  alloc_map_create(heap->alloc_map, heap->memory, WORD_SIZE, bytes);
  
  void *start_of_pages = (void *) ((size_t) heap->alloc_map + alloc_map_size);
  create_pages(heap->memory, start_of_pages, number_of_pages, PAGE_SIZE, heap);
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
  assert(h != NULL);
  if(h==NULL) return;
  free(h);
}


void 
h_delete_dbg(heap_t *h, void *dbg_value)
{
  assert(h != NULL);
  if(h==NULL) return;
  void *stack_top = get_stack_top();
  size_t number_of_ptrs = get_number_of_ptrs_in_stack(h, stack_top);
  void **array[number_of_ptrs];
  size_t actual_nr_of_ptrs = get_ptrs_from_stack(h, stack_top, array, number_of_ptrs);
  for(size_t i= 0; i < actual_nr_of_ptrs; ++i)
    {
      *array[i] = dbg_value;
    }
  h_delete(h);
}

/**
 *  @brief Recursively gets the number of all active pointers in the heap 
 *
 *  @param  h a pointer to the heap
 *  @param  ptr a pointer to be searched recursively from
 *
 *  @return the number of all active pointers in the heap 
 */
size_t
get_number_of_active_heap_ptrs_rec(heap_t *h, void *ptr)
{
  if(get_header_type(ptr) != STRUCT_REP)
    {
      return 0;
    }
  else 
    {
      size_t num_ptrs = get_number_of_pointers_in_struct(ptr);
      size_t result = 0;
      void **array[num_ptrs];
      bool success = get_pointers_in_struct(ptr, array);
      if(!success) return 0;
      
      for(size_t i = 0; i < num_ptrs; ++i)
        {
          if(alloc_map_ptr_used(h->alloc_map, *array[i]))
            {
              ++result;
              result += get_number_of_active_heap_ptrs_rec(h, *array[i]);
            }
        }
      return result;
    }
}

/**
 *  @brief Gets the number of active pointers on the stack and in the heap 
 *
 *  @param  h a pointer to the heap
 *  @param  original_top the top of the stack to search
 *
 *  @return the number of pointer found on stack and the heap
 */
size_t
get_number_of_active_ptrs(heap_t *h, void *original_top)
{
  void *stack_top = original_top;
  void *stack_bottom = (void *)*environ;
  void *heap_start = h->memory;
  void *heap_end = (h->memory + h->size);
  void **pointer = stack_find_next_ptr(&stack_bottom, stack_top, heap_start, heap_end);

  size_t i = 0;
  while (pointer != NULL)
    {
      if(alloc_map_ptr_used(h->alloc_map, *pointer))
        {
          i += 1;
          i += get_number_of_active_heap_ptrs_rec(h, *pointer);
        }
      pointer = stack_find_next_ptr(&stack_bottom, stack_top, heap_start, heap_end);
    }
  return i;
}

/**
 *  @brief Recursively gets all the active pointers in the heap and puts them into an array
 *
 *  @param  h a pointer to the heap
 *  @param  ptr a pointer to be searched recursively from
 *  @param  array an array of double pointers 
 *  @param  index where in the array to put found pointers
 *
 */
void
get_active_heap_ptrs_rec(heap_t *h, void *ptr, void **array[], size_t *index)
{
  if(get_header_type(ptr) != STRUCT_REP)
    {
      return;
    }
  else 
    {
      size_t num_ptrs = get_number_of_pointers_in_struct(ptr);
      if(num_ptrs == 0) return;
      void **temp_array[num_ptrs];
      bool success = get_pointers_in_struct(ptr, temp_array);
      if(!success) return;
      size_t num_valid_ptrs = 0;
      void ***sub_array =  &(array[*index]);
      for(size_t i = 0; i < num_ptrs; ++i)
        {
          if(alloc_map_ptr_used(h->alloc_map, *temp_array[i]))
            {
              array[*index] = temp_array[i];
              *index += 1;
              ++num_valid_ptrs;
            }
        }
      
      for(size_t i = 0; i < num_valid_ptrs; ++i)
        {
          get_active_heap_ptrs_rec(h, *sub_array[i], array, index);
        }
    }
}

/**
 *  @brief Gets all the active pointers on the stack and in the heap and puts them into an array
 *
 *  @param  h a pointer to the heap
 *  @param  original_top the top of the stack to search
 *  @param  array an array of double pointers 
 *  @param  array_size the size of the array
 *
 *  @return the number of pointer found on stack and the heap
 */
size_t
get_active_ptrs(heap_t *h, void *original_top, void **array[], size_t array_size)
{
  for(size_t i = 0; i < array_size; ++i) array[i] = NULL;

  size_t num_stack_ptrs = get_ptrs_from_stack(h, original_top, array, array_size);
  size_t index = num_stack_ptrs;
  for(size_t i = 0; i < num_stack_ptrs; ++i)
    {
      get_active_heap_ptrs_rec(h, *array[i], array, &index);
    }
  return num_stack_ptrs;
}


/**
 *  @brief Gets the number of pointer found on stack
 *
 *  @param  h a pointer to the heap
 *  @param  original_top the top of the stack to search
 *
 *  @return the number of pointer found on stack
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
      if(alloc_map_ptr_used(h->alloc_map, *pointer))
        {
          i += 1;
        }
      pointer = stack_find_next_ptr(&stack_bottom, stack_top, heap_start, heap_end);
    }
  return i;
}


/**
 *  @brief Puts the pointers found on the stack in an array
 *
 *  @param  h a pointer to the heap
 *  @param  original_top the top of the stack to search
 *  @param  array an empty array of double pointers 
 *  @param  array_size the size of the array 
 *
 *  @return the number of pointer found on stack
 */
size_t
get_ptrs_from_stack(heap_t *h, void *original_top, void **array[], size_t array_size)
{
  void *stack_top = original_top;
  void *stack_bottom = (void *)*environ;
  void *heap_start = h->memory;
  void *heap_end = (h->memory + h->size);

  void **pointer = stack_find_next_ptr(&stack_bottom, stack_top, heap_start, heap_end);

  size_t i = 0;
  while (pointer != NULL)
    {
      if(alloc_map_ptr_used(h->alloc_map, *pointer))
        {       
          assert(i < array_size && "There are more ptrs in stack than we thought!");
          array[i] = pointer;

          i += 1;
        }
      pointer = stack_find_next_ptr(&stack_bottom, stack_top, heap_start, heap_end);
    } 
  assert(i == array_size && "There are less ptrs in stack than we thought!"); 
  return i;  
}

int
get_ptr_page(heap_t *h, void * ptr)
{
  int number_of_pages = h->number_of_pages;
  for (int i = 0; i < number_of_pages; ++i) 
    {
      page_t *current = h->pages[i];
      if(ptr >= page_get_start(current) && ptr < page_get_end(current))
        { 
          return i;
        }
    }
  return -1;
}


int
find_next_active_page(heap_t *h, size_t index)
{
  for(int i = index; i < (int)h->number_of_pages; ++i)
    {
      if(h->pages[i]->type == ACTIVE)
        {
          return i; 
        }
    }
  return -1;
}

size_t
number_of_passive_pages(heap_t *h)
{
  size_t count = 0; 
  for(int i =0; i < (int)h->number_of_pages; ++i)
    {
      if(h->pages[i]->type == PASSIVE)
        {
          count += 1;
        }
    }
  return count;
}

page_t *
find_first_passive_page(heap_t *h)
{
  for(int i =0; i < (int)h->number_of_pages; ++i)
    {
      if(h->pages[i]->type == PASSIVE)
        {
          return h->pages[i]; 
        }
    }
  return NULL;
}


bool
run_gc_if_above_threshold(heap_t *h, size_t bytes) //Bra namn ???
{
  if(((float)h_used(h)+bytes)/(float)h->size > h->gc_threshold)
    {  
      size_t cleaned = h_gc(h);
      if (cleaned == 0)
        {
          return true;
        }
      else if(((float)h_used(h)+bytes)/(float)h->size > h->gc_threshold)
        {
          return true;
        }   
    }
  return false;
}


/**
 *  @brief Allocates bytes amount of data on the heap
 *
 *  The function will garbage collect if the heap will reach the threshold after allocation, 
 *  or if only one passive page remains.  
 *
 *  @param  h a pointer to the heap
 *  @param  bytes the size of the data (including header) to be allocated
 *  @return a pointer to the allocated data if successful or NULL otherwise 
 */
void *
h_alloc(heap_t * h, size_t bytes)
{
  if (run_gc_if_above_threshold(h, bytes))
    {
      return NULL;
    }

  if(bytes < SMALLEST_ALLOC_SIZE)
    {
      bytes = SMALLEST_ALLOC_SIZE;
    }

  if(bytes % WORD_SIZE != 0)
    {
      bytes += WORD_SIZE - (bytes % WORD_SIZE);
    }

  int index_next_act = find_next_active_page(h, 0);
  page_t *page_to_write_to;
  while (index_next_act >= 0) 
    {
      if ( page_get_avail(h->pages[index_next_act])> bytes)
        {
          page_to_write_to = h->pages[index_next_act];
          break;
        }
      else 
        {
          index_next_act = find_next_active_page(h, index_next_act + 1);
        }      
    }

  if(index_next_act < 0) 
    {
      if (number_of_passive_pages(h) <= 1)
        {
          bool not_cleaned_enough = run_gc_if_above_threshold(h, bytes);
          if(not_cleaned_enough)
            {
              return NULL;
            }
          else if (number_of_passive_pages(h) <= 1)
            {
              return NULL;
            }        
        }
      page_to_write_to = find_first_passive_page(h);
      page_to_write_to->type = ACTIVE;
    }

  
  void *page_bump = page_get_bump(page_to_write_to);
  void * ptr_to_write_to = page_bump;
  page_move_bump(page_to_write_to, bytes);
  return ptr_to_write_to; 
}


void *
h_alloc_struct(heap_t * h, char * layout)
{
  assert(h != NULL);
  assert(layout != NULL);
  assert(*layout != '\0');
  if(h == NULL || layout == NULL || *layout == '\0') return NULL;
  
  size_t size = get_struct_size(layout);
  assert(size <= PAGE_SIZE);
  assert(size > 0);
  if(size > PAGE_SIZE || size == 0) return NULL;
 
  void * ptr = h_alloc(h, size);
  if (ptr == NULL) return NULL;
  
  void * return_ptr = create_struct_header(layout, ptr);
  alloc_map_set(h->alloc_map, return_ptr, true); 
  return return_ptr;
}


void *
h_alloc_data(heap_t * h, size_t bytes)
{
  assert(bytes > 0);
  assert(h != NULL);
  if(h == NULL || bytes == 0) return NULL;
  
  size_t size = get_data_size(bytes);
  assert(size <= PAGE_SIZE);
  assert(size > 0);
  if(size > PAGE_SIZE || size == 0) return NULL;
  void * ptr = h_alloc(h, size);
  if (ptr == NULL) return NULL;
  
  void * return_ptr = create_data_header(bytes, ptr);
  alloc_map_set(h->alloc_map, return_ptr, true); 
  return return_ptr;
}


/**
 *  @brief reallocates data from one page to another
 *
 *  @param  h a pointer to the heap
 *  @param  ptr_to_data a pointer to the data (without header) to be reallocated
 *
 *  @return a pointer to the reallocaded data
 */
void *
h_alloc_raw(heap_t *h, void *ptr_to_data)
{
  assert(ptr_to_data != NULL);
  size_t raw_size = get_existing_size(ptr_to_data);
  if(raw_size < SMALLEST_ALLOC_SIZE)
    {
      raw_size = SMALLEST_ALLOC_SIZE;
    }
  if(raw_size % WORD_SIZE != 0)
    {
      raw_size += WORD_SIZE - (raw_size % WORD_SIZE);
    }
  int index_next_act = find_next_active_page(h, 0);
  page_t *page_to_write_to;
  while (index_next_act >= 0) 
    {
      if ( page_get_avail(h->pages[index_next_act])> raw_size)
        {
          page_to_write_to = h->pages[index_next_act];
          break;
        }
      else 
        {
          index_next_act = find_next_active_page(h, index_next_act + 1);
        }
    }
  if(index_next_act < 0) 
    {
      page_to_write_to = find_first_passive_page(h);
      page_to_write_to->type = ACTIVE;
    }
 
  void *page_bump = page_get_bump(page_to_write_to);
  void * ptr_to_write_to = page_bump;
  page_move_bump(page_to_write_to, raw_size);

  size_t data_size = get_existing_data_size(ptr_to_data);
  void *ptr_to_moved_data = copy_header(ptr_to_data, ptr_to_write_to);
  forward_header(ptr_to_data, ptr_to_moved_data);

  void * return_ptr = memcpy(ptr_to_moved_data, ptr_to_data, data_size);
  alloc_map_set(h->alloc_map, return_ptr, true); 
  alloc_map_set(h->alloc_map, ptr_to_data, false); 
  return return_ptr;
}


void * get_stack_top(){
  return __builtin_frame_address(0);
}


void
set_active_to_transition(heap_t *h)
{
  for(int i =0; i < (int)h->number_of_pages; ++i)
    {
      if(h->pages[i]->type == ACTIVE)
        {
          h->pages[i]->type = TRANSITION;
        }
    }
}


/**
 *  @brief Reallocates pointers from a reallocated struct  
 *
 *  If a pointer in the array is within the range the offset is added to it 
 *  to give it the correct address
 *
 *  @param array an array of double pointers to addresses on the heap
 *  @param start_index the index of the array where to start forwarding
 *  @param array_size the size of the array
 *  @param ptr_to_original_data a pointer to the original data address 
 *  @param ptr_to_new_data a pointer to the new data address
 */
void
forward_internal_array_ptrs_with_offset(void **array[], 
                                        size_t start_index,
                                        size_t array_size,
                                        void *ptr_to_original_data,
                                        void *ptr_to_new_data)
{
  long offset = (long) ptr_to_new_data - (long) ptr_to_original_data;
  void *lower_range = ptr_to_original_data;
  void *upper_range = (void *) ((unsigned long) lower_range + get_existing_data_size(ptr_to_new_data));
  
  for(size_t index = start_index; index < array_size; ++index)
    {
      if(lower_range < *array[index] && *array[index] < upper_range)
        {
          *array[index] = *array[index] + offset;
        }
    }
}

size_t 
h_gc(heap_t *h)
{
  return h_gc_dbg(h, SAFE_STACK);
}

void
set_unsafe_pages(heap_t *h, void **array[], size_t array_size)
{
  for(size_t i = 0; i < array_size; ++i)
    {
      if((alloc_map_ptr_used(h->alloc_map, *array[i])))
        {
          int index = get_ptr_page(h, *array[i]);
          if(h->pages[index]->type == TRANSITION)
            {
              h->pages[index]->type = UNSAFE;
            }
        }
    }
}

void
set_unsafe_pages_to_active(heap_t *h)
{
  for(int i =0; i < (int)h->number_of_pages; ++i)
    {
      if(h->pages[i]->type == UNSAFE)
        { 
          h->pages[i]->type = ACTIVE;
        }
    }
}


size_t 
h_gc_dbg(heap_t *h, bool unsafe_stack)
{
  assert(h != NULL);
  if(h == NULL) return 0;
  //Dump_registers();

  size_t used_before_gc = h_used(h);
  set_active_to_transition(h);

  void *stack_top = get_stack_top();
  size_t num_active_ptrs = get_number_of_active_ptrs(h, stack_top);
  void **array_of_found_ptrs[num_active_ptrs];
  size_t num_stack_ptrs = get_active_ptrs(h, stack_top,  array_of_found_ptrs, num_active_ptrs);
 
  if(unsafe_stack == UNSAFE_STACK)
    {
      set_unsafe_pages(h, array_of_found_ptrs, num_stack_ptrs);
    }

  for(size_t page_nr = 0; page_nr < h->number_of_pages; ++page_nr)
    {
      if (page_get_type(h->pages[page_nr]) == TRANSITION)
        {
          for(size_t ptr_index = 0; ptr_index < num_active_ptrs; ++ptr_index)
            {
              void *ptr_to_original_data = *array_of_found_ptrs[ptr_index];
              if (get_ptr_page(h, ptr_to_original_data) == (int) page_nr) 
                {
                  void *ptr_to_new_data = NULL;
                  if(get_header_type(ptr_to_original_data) == FORWARDING_ADDR)
                    {
                      ptr_to_new_data = get_forwarding_address(ptr_to_original_data);
                    }
                  else
                    {
                      ptr_to_new_data = h_alloc_raw(h, *array_of_found_ptrs[ptr_index]);
                      if(get_header_type(ptr_to_new_data) == STRUCT_REP)
                        {
                          forward_internal_array_ptrs_with_offset(array_of_found_ptrs, ptr_index, 
                                                                  num_active_ptrs, 
                                                                  ptr_to_original_data,
                                                                  ptr_to_new_data);
                        }
                    }
                  *array_of_found_ptrs[ptr_index] = ptr_to_new_data;
                }
            }
          page_set_type(h->pages[page_nr], PASSIVE);
          page_reset(h->pages[page_nr]);
        }
    }
  set_unsafe_pages_to_active(h);
  size_t used_after_gc = h_used(h);
  size_t collected = used_before_gc - used_after_gc;
  return collected;
}


size_t 
h_avail(heap_t *h)
{
  assert(h != NULL);
  if(h == NULL) return 0;
  size_t avail = 0;
  int number_of_pages = h->number_of_pages;
  for (int i = 0; i < number_of_pages; i++) 
    {
      avail += page_get_avail(h->pages[i]);  
    }
  return avail;
}


size_t 
h_used(heap_t *h)
{
  assert(h != NULL);
  if(h == NULL) return 0;
  size_t used = 0;
  int number_of_pages = h->number_of_pages;
  for (int i = 0; i < number_of_pages; i++) 
    {
      used += page_get_used(h->pages[i]);  
    }
  return used;
}


size_t
h_size(heap_t *h)
{
  assert(h != NULL);
  if(h == NULL) return 0;
  return h->size; 
}

