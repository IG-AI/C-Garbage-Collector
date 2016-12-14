#include <stdio.h>
#include <stdlib.h>
#include "gc.h"

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

void 
test_h_init ()
{
  int test_size = 2048;
  heap_t *test_h_init_heap = h_init(test_size, true, 1);
  CU_ASSERT(test_h_init_heap != NULL);
  
  //h_delete(test_h_init_heap);
}

void 
test_pages ()
{
  
  int test_size = 6144;
  int page_size = 2048;
  heap_t *test_pages_heap = h_init(test_size, true, 1);
  CU_ASSERT(get_pages(test_pages_heap, 0) >= get_memory(test_pages_heap) ); 
  CU_ASSERT(get_pages(test_pages_heap, 1) <= (get_memory(test_pages_heap) + test_size - page_size) );
  printf("\nMem start:  %p  \n", get_memory(test_pages_heap));  
  printf("First page: %p \n", get_pages(test_pages_heap,0));
  printf("Second page:%p\n", get_pages(test_pages_heap,1));
  printf("Third page:%p\n", get_pages(test_pages_heap,2));
  printf("Mem end:    %p  \n", get_memory(test_pages_heap) + test_size);
  
  h_delete(test_pages_heap);
}

int
main (int argc, char *argv[])
{
  CU_pSuite suite1 = NULL;

  if (CU_initialize_registry() != CUE_SUCCESS)
    {
      return CU_get_error();
    }

  suite1 = CU_add_suite("Heap Test", NULL, NULL);

  if ((CU_add_test(suite1, "test_h_init()", test_h_init) == NULL) ||
      (CU_add_test(suite1, "test_page()", test_pages) == NULL)
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
