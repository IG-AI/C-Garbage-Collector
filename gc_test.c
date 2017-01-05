#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "gc.h"
#include "gc_hidden.h"
#include "header.h"

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#define WORD_SIZE 8
#define SMALLEST_HEAP_SIZE 4096

/* struct heap{ */
/*   void * memory; */
/*   size_t size; */
/*   bool unsafe_stack; */
/*   float gc_threshold; */
/*   int number_of_pages; */
/*   page_t * pages[]; */
/* }; */


/* struct page{ */
/*   void * start; */
/*   void * bump; */
/*   int size; */
/* }; */

#define DEFAULT_SIZE = 2*2048;

struct test_struct{
  int first;
  void * second;
  int third;
};

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

/*============================================================================
 *                             h_init TESTING SUITE
 *===========================================================================*/
void
test_h_init_size_zero()
{
  heap_t *h = h_init(0, SAFE_STACK, 1);
  CU_ASSERT(h == NULL);
}

void
test_h_init_size_one()
{
  heap_t *h = h_init(1, SAFE_STACK, 1);
  CU_ASSERT(h == NULL);
}

void
test_h_init_size_eight()
{
  heap_t *h = h_init(8, SAFE_STACK, 1);
  CU_ASSERT(h == NULL);
}

void
test_h_init_size_2047()
{
  heap_t *h = h_init(2047, SAFE_STACK, 1);
  CU_ASSERT(h == NULL);
}

void
test_h_init_size_one_below_smallest()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE - 1, SAFE_STACK, 1);
  CU_ASSERT(h == NULL);
}

void
test_h_init_size_one_above_smallest()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE + 1, SAFE_STACK, 1);
  CU_ASSERT(h == NULL);
}

void
test_h_init_size_odd()
{
  heap_t *h = h_init(10*SMALLEST_HEAP_SIZE - 11, SAFE_STACK, 1);
  CU_ASSERT(h == NULL);
}

void
test_h_init_size_max()
{
  heap_t *h = h_init(SIZE_MAX/2048 * 2048, SAFE_STACK, 1);
  CU_ASSERT(h == NULL); // TODO: Define behaviour
}

void
test_h_init_size_one_below_max()
{
  heap_t *h = h_init(SIZE_MAX - 1, SAFE_STACK, 1);
  CU_ASSERT(h == NULL);
}

void
test_h_init_threshold_zero()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0);
  CU_ASSERT(h == NULL);
}

void
test_h_init_threshold_negative()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, -1);
  CU_ASSERT(h == NULL);
}

void
test_h_init_threshold_larger_than_one()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 1.1);
  CU_ASSERT(h == NULL);
}

void
test_h_init_threshold_close_to_edge()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.9999);
  CU_ASSERT(h != NULL);
  h_delete(h);
}

void
test_h_init_threshold_edge()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 1);
  CU_ASSERT(h != NULL);
  h_delete(h);
}

void
test_h_init_success()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  CU_ASSERT(h != NULL);
  h_delete(h);
}


/*============================================================================
 *                             ... TESTING SUITE
 *===========================================================================*/


void 
test_pages ()
{
  time_t t;
  srand( (unsigned) time(&t));

  for (int i = 1; i <= 1; i++) {
    int rand_int = (rand() %20) + 1;
    int test_size = 2048 * rand_int;
    heap_t *test_pages_heap = h_init(test_size, true, 1);
    void *pages_start = get_memory(test_pages_heap) + test_size;
    void *pages_end = (get_memory(test_pages_heap) + test_size + (sizeof(page_t) * heap_get_number_of_pages(test_pages_heap)) );
    for(int j = 0; j < rand_int; j++){
      void * ptr = (void *)test_pages_heap->pages[j];
      CU_ASSERT( ptr >= pages_start && ptr < pages_end);
      //printf("\nMemory end:%lu\nPointer:   %lu\nPages end: %lu\n",
      //       (long unsigned)memory_end, (long unsigned)ptr, (long unsigned)pages_end);
    }

    //printf("\nMem start:  %p  \n", get_memory(test_pages_heap));  
    //printf("First page: %p \n", get_page_start(test_pages_heap->pages[0]));
    //printf("Second page:%p\n", get_page_start(test_pages_heap->pages[1]));
    //printf("Third page: %p\n", get_page_start(test_pages_heap->pages[2]));
    //printf("Mem end:    %p  \n", get_memory(test_pages_heap) + test_size);
 
    h_delete(test_pages_heap);
  }
}

void
test_get_ptr_page()
{
  int test_size = 6144;
  heap_t *test_h_alloc_heap = h_init(test_size, true, 1); 

  void * ptr1 = h_alloc_struct(test_h_alloc_heap, "i");
  write_int_to_heap(ptr1, 6);

  CU_ASSERT(get_ptr_page(test_h_alloc_heap, ptr1) != -1);
}

void
test_h_alloc()
{
  int test_size = 6144;
  heap_t *test_h_alloc_heap = h_init(test_size, true, 1);

  void * ptr1 = h_alloc_struct(test_h_alloc_heap, "i");
  write_int_to_heap(ptr1, 6);

  void * ptr2 = h_alloc_struct(test_h_alloc_heap, "i");
  write_int_to_heap(ptr2, 9);

  void * ptr4 = h_alloc_data(test_h_alloc_heap, 2040);

  void * ptr5 = h_alloc_struct(test_h_alloc_heap, "i*i");
  struct test_struct ts = ( (struct test_struct) {1, &ptr5 ,3}); 
  *(struct test_struct *)ptr5 = ts;
  //printf("\nTest struct:  %d %p %d  \n", (*(struct test_struct *)ptr5).first, 
  //       (*(struct test_struct *)ptr5).second,
  //       (*(struct test_struct *)ptr5).third );



  void * ptr3 = h_alloc_struct(test_h_alloc_heap, "*");
  void * test_pointer = malloc(10);
  write_pointer_to_heap(&ptr3, test_pointer);

  //printf("\nTest pointer:  %p  \n", test_pointer);  
  //printf("Pointer 2    : %p \n", (void *)ptr2 );
  //printf("\nAvail: %lu \n", h_avail(test_h_alloc_heap));


  CU_ASSERT(*(int *)ptr1 == 6);
  CU_ASSERT(*(int *)ptr2 == 9);
  CU_ASSERT_PTR_EQUAL(test_pointer, ptr3);
  CU_ASSERT(ptr4 != NULL);
  CU_ASSERT(ptr5 != NULL);

  h_delete(test_h_alloc_heap);
  free(test_pointer);
}



void
test_h_alloc_threshold ()
{
  int test_size = 6144;
  heap_t *test_h_alloc_heap = h_init(test_size, true, 0.3);

  void *ptr1 = h_alloc_data(test_h_alloc_heap, 5);
  void *ptr2 = h_alloc_data(test_h_alloc_heap, 1990);

  CU_ASSERT(ptr1 != NULL);
  CU_ASSERT(ptr2 == NULL);
  
  h_delete(test_h_alloc_heap);

  heap_t *test_h_alloc_heap_passive = h_init(test_size, true, 1);
  void *ptr3 = h_alloc_data(test_h_alloc_heap_passive, 2000);
  void *ptr4 = h_alloc_data(test_h_alloc_heap_passive, 2000);
  void *ptr5 = h_alloc_data(test_h_alloc_heap_passive, 2000);

  CU_ASSERT(ptr3 != NULL);
  CU_ASSERT(ptr4 != NULL);
  CU_ASSERT(ptr5 == NULL);
  
}




/*
//TODO FAILS SOMETIMES ?? 
void 
test_h_size()
{
  time_t t;
  srand( (unsigned) time(&t));

  int test_size = 2048;
  int alloc_size = 2040;
  heap_t *test_h_size_heap = h_init(test_size, true, 1);
  h_alloc_data(test_h_size_heap, alloc_size);
    
  CU_ASSERT(h_avail(test_h_size_heap) == ( (size_t) (test_size) - get_data_size(alloc_size) ) );
  CU_ASSERT(h_used(test_h_size_heap) == ( (size_t) get_data_size(alloc_size) ) );
  CU_ASSERT(h_size(test_h_size_heap) == ( (size_t) test_size) );
  h_delete(test_h_size_heap);  

  
  for (int i = 1; i <= 42; i++) {

    int test_size = 2048;
    int rand_nr = (rand() % 2000) + 1;
  
    heap_t *test_h_size_heap = h_init(test_size, true, 1);
    h_alloc_data(test_h_size_heap, rand_nr);

    if(rand_nr % WORD_SIZE != 0){
      rand_nr += WORD_SIZE - (rand_nr % WORD_SIZE);
    }
    
    CU_ASSERT(h_avail(test_h_size_heap) == ( (size_t) (test_size) - get_data_size(rand_nr) ) );
    CU_ASSERT(h_used(test_h_size_heap) == ( (size_t) get_data_size(rand_nr) ) );
    CU_ASSERT(h_size(test_h_size_heap) == ( (size_t) test_size) );
    h_delete(test_h_size_heap);

  }
  
}
*/
extern char **environ;


void
test_get_number_of_ptrs_in_stack()
{
  int test_size = 2048*3;
  heap_t *h = h_init(test_size, true, 1);

  void * ptr1 = h_alloc_struct(h, "i");
  write_int_to_heap(ptr1, 6);
  void * ptr2 = h_alloc_struct(h, "i");
  write_int_to_heap(ptr2, 9); 
  
  void *stack_top = get_stack_top();
  size_t number_of_ptrs = get_number_of_ptrs_in_stack(h, stack_top);
  // printf("\nNum of ptrs: %lu \n", number_of_ptrs);
  CU_ASSERT (number_of_ptrs > 0);
  h_delete(h); 

}
void
test_ptrs_from_stack()
{
  int test_size = 2048*3;
  heap_t *h = h_init(test_size, true, 1);

  void *ptr1 = h_alloc_struct(h, "i");
  write_int_to_heap(ptr1, 6);
  void *ptr2 = h_alloc_struct(h, "i");
  write_int_to_heap(ptr2, 9); 
  
  void *stack_top = get_stack_top();
  size_t number_of_ptrs = get_number_of_ptrs_in_stack(h, stack_top);
  void **collection[number_of_ptrs];
  // printf("\n%lu", number_of_ptrs);

  get_ptrs_from_stack(h, stack_top, collection, number_of_ptrs);
  CU_ASSERT(collection[0] != NULL );
  /* printf("\n1: %p\n", collection[0]);
  printf("\n2: %p\n", collection[1]);
  printf("\n1: %d\n", *(int*)*collection[0]);
  printf("\n2: %d\n",*(int *)*collection[1]);*/
  CU_ASSERT(*(int *) *collection[1] == 6); 
  CU_ASSERT(*(int *) *collection[0] == 9); 
  h_delete(h); 

}


void 
test_h_delete_dbg(void)
{
  int test_size = 6144;
  heap_t *h = h_init(test_size, true, 1);
  
  void *ptr1 = h_alloc_struct(h, "i");
  write_int_to_heap(ptr1, 6); 
  void *ptr2 = h_alloc_struct(h, "i");
  write_int_to_heap(ptr2, 9);

  
  h_delete_dbg(h, NULL);
 
  CU_ASSERT( ptr1 == NULL);
  CU_ASSERT( ptr2 == NULL);
} 



void
test_h_gc()
{
  time_t t;
  srand( (unsigned) time(&t));

  int test_size = 6144;
  heap_t *h = h_init(test_size, true, 1);
  void * ptr1 = h_alloc_struct(h, "i**");
  void *data_ptr = h_alloc_data(h, 16);
  write_int_to_heap(ptr1, 6);
  write_pointer_to_heap((void *)((unsigned long) ptr1 + sizeof(int)), (void *) 3UL);
  write_pointer_to_heap((void *)((unsigned long) ptr1 + sizeof(int) + sizeof(void *)), data_ptr);
  size_t cleaned = h_gc(h); 
  CU_ASSERT(*(int *) ptr1 == 6);
  CU_ASSERT(*(unsigned long *)(ptr1 + sizeof(int)) == 3UL);
  CU_ASSERT(cleaned == 0);
  h_delete(h);   
}


int
main (int argc, char *argv[])
{
  CU_pSuite suite1 = NULL;
  CU_pSuite suite_h_init = NULL;
  if (CU_initialize_registry() != CUE_SUCCESS)
    {
      return CU_get_error();
    }


  // ********************* get_h_init SUITE ******************  //
  suite_h_init = CU_add_suite("Tests function h_init()", NULL, NULL);
  if (suite_h_init == NULL) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if ( (NULL == CU_add_test(suite_h_init
                            , "size 0"
                            , test_h_init_size_zero) )
       || (NULL == CU_add_test(suite_h_init
                               , "size 1"
                               , test_h_init_size_one) )
       || (NULL == CU_add_test(suite_h_init
                               , "size 8"
                               , test_h_init_size_eight) )
       || (NULL == CU_add_test(suite_h_init
                               , "size 2047"
                               , test_h_init_size_2047) )
       || (NULL == CU_add_test(suite_h_init
                               , "one below smallest"
                               , test_h_init_size_one_below_smallest) )
       || (NULL == CU_add_test(suite_h_init
                               , "one above smallest"
                               , test_h_init_size_one_above_smallest) )
       || (NULL == CU_add_test(suite_h_init
                               , "size odd"
                               , test_h_init_size_odd) )
    || (NULL == CU_add_test(suite_h_init
                               , "size max"
                               , test_h_init_size_max) )
       || (NULL == CU_add_test(suite_h_init
                               , "size one below max"
                               , test_h_init_size_one_below_max) )
       || (NULL == CU_add_test(suite_h_init
                               , "threshold zero"
                               , test_h_init_threshold_zero) )
       || (NULL == CU_add_test(suite_h_init
                               , "threshold negative"
                               , test_h_init_threshold_negative) )
       || (NULL == CU_add_test(suite_h_init
                               , "threshold above 1"
                               , test_h_init_threshold_larger_than_one) )
       || (NULL == CU_add_test(suite_h_init
                               , "threshold close to edge"
                               , test_h_init_threshold_close_to_edge) )
       || (NULL == CU_add_test(suite_h_init
                               , "threshold edge"
                               , test_h_init_threshold_edge) )
       || (NULL == CU_add_test(suite_h_init
                               , "success"
                               , test_h_init_success) )
       
    )
    {
      CU_cleanup_registry();
      return CU_get_error();
    }






  suite1 = CU_add_suite("Heap Test", NULL, NULL);

  if (
      (CU_add_test(suite1, "test_page()", test_pages) == NULL) ||
      (CU_add_test(suite1, "test_h_alloc_struct/data()", test_h_alloc) == NULL) ||
      (CU_add_test(suite1, "test_ptrs_from_stack", test_ptrs_from_stack) == NULL) ||
      (CU_add_test(suite1, "test_number_of_ptrs_in_stack", test_get_number_of_ptrs_in_stack) == NULL) ||
      (CU_add_test(suite1, "test_h_delete_dbg", test_h_delete_dbg) == NULL) ||
      (CU_add_test(suite1, "test_h_gc)", test_h_gc) == NULL)||
      (CU_add_test(suite1, "test_h_alloc_threshold)", test_h_alloc_threshold) == NULL)||
      (CU_add_test(suite1, "test_get_ptr_page)", test_get_ptr_page) == NULL)
      )
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return CU_get_error();
}
