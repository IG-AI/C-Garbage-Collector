#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include "alloc_map.h"

struct alloc_map 
{
  void * start_addr;
  size_t word_size;
  size_t map_size;
  uint8_t bits[];
};


void 
test_alloc_map_create() 
{

  int type_size = sizeof(int);
  typedef int type_t;
  int block_size = 256;
  size_t i = (block_size*type_size);
  type_t *start_addr = malloc(i*sizeof(type_t));
  alloc_map_t *alloc_map_create_alloc_map = malloc(alloc_map_mem_size_needed(type_size, i));
  alloc_map_create(alloc_map_create_alloc_map, start_addr,type_size, i);
  
  //checks creation and values in the struct
  CU_ASSERT(alloc_map_create_alloc_map->word_size == 4);
  CU_ASSERT(alloc_map_create_alloc_map->start_addr == start_addr);
  CU_ASSERT(alloc_map_create_alloc_map->map_size == 256);
  
  CU_ASSERT_FALSE(alloc_map_ptr_used(alloc_map_create_alloc_map, (void *)&(start_addr[1])));

  free(start_addr);
  free(alloc_map_create_alloc_map);
}


void
test_alloc_map_sets()
{
  int type_size = sizeof(int);
  typedef int type_t;
  int block_size = 2048;
  size_t i = (block_size*type_size);
  type_t *start_addr = malloc(i*sizeof(type_t));
  alloc_map_t *alloc_map = malloc(alloc_map_mem_size_needed(type_size, i));
  alloc_map_create(alloc_map, start_addr,type_size, i);

  CU_ASSERT_FALSE(alloc_map_ptr_used(alloc_map, (void *)&(start_addr[1])));
  alloc_map_set(alloc_map, (void *)&(start_addr[1]), true);
  CU_ASSERT_TRUE(alloc_map_ptr_used(alloc_map, (void *)&(start_addr[1])));

  alloc_map_print_in_use(alloc_map);

  alloc_map_set(alloc_map, (void *)&(start_addr[1]), false);
  CU_ASSERT_FALSE(alloc_map_ptr_used(alloc_map, (void *)&(start_addr[1])));


  free(start_addr);
  free(alloc_map);
}

void
test_alloc_map_sets_edge()
{
  int type_size = sizeof(size_t);
  typedef size_t type_t;
  int block_size = 1024;
  size_t i = (block_size*type_size);
  type_t *start_addr = malloc(i*sizeof(type_t));
  alloc_map_t *alloc_map = malloc(alloc_map_mem_size_needed(type_size, i));
  alloc_map_create(alloc_map, start_addr,type_size, i);

  
   //Check for lowest valid address
  CU_ASSERT_FALSE(alloc_map_ptr_used(alloc_map, (void *)&(start_addr[0])));

  CU_ASSERT_TRUE(alloc_map_set(alloc_map, (void *)&(start_addr[0]), true));
  CU_ASSERT_TRUE(alloc_map_ptr_used(alloc_map, (void *)&(start_addr[0])));

  CU_ASSERT_TRUE(alloc_map_set(alloc_map, (void *)&(start_addr[0]), false));
  CU_ASSERT_FALSE(alloc_map_ptr_used(alloc_map, (void *)&(start_addr[0])));

 
  //Check for one below lowest valid address

  CU_ASSERT_FALSE(alloc_map_ptr_used(alloc_map, (void *)&(start_addr[0])-1));

  CU_ASSERT_FALSE(alloc_map_set(alloc_map, (void *)&(start_addr[0])-1, true));
  CU_ASSERT_FALSE(alloc_map_ptr_used(alloc_map, (void *)&(start_addr[0])-1));

  CU_ASSERT_FALSE(alloc_map_set(alloc_map, (void *)&(start_addr[0])-1, false));
  CU_ASSERT_FALSE(alloc_map_ptr_used(alloc_map, (void *)&(start_addr[0])-1));
  
  
  //  Check for highest valid address
  CU_ASSERT_TRUE(alloc_map_set(alloc_map, (void *)&(start_addr[block_size-1]), true));
  CU_ASSERT_TRUE(alloc_map_ptr_used(alloc_map, (void *)&(start_addr[block_size-1])));

  CU_ASSERT_TRUE(alloc_map_set(alloc_map, (void *)&(start_addr[block_size-1]), false));
  CU_ASSERT_FALSE(alloc_map_ptr_used(alloc_map, (void *)&(start_addr[block_size-1])));

  // Check for one above highest valid address
  CU_ASSERT_FALSE(alloc_map_set(alloc_map, (void *)&(start_addr[block_size]), true));
  CU_ASSERT_FALSE(alloc_map_ptr_used(alloc_map, (void *)&(start_addr[block_size])));

  CU_ASSERT_FALSE(alloc_map_set(alloc_map, (void *)&(start_addr[block_size]), false));
  CU_ASSERT_FALSE(alloc_map_ptr_used(alloc_map, (void *)&(start_addr[block_size])));



  free(start_addr);
  free(alloc_map);
}


int
main (int argc, char *argv[])
{
  CU_pSuite suite1 = NULL;

  if (CU_initialize_registry() != CUE_SUCCESS)
    {
      return CU_get_error();
    }

  suite1 = CU_add_suite("Alloc_map Test", NULL, NULL);

  if (
       (CU_add_test(suite1, "test_alloc_map_test()", test_alloc_map_create) == NULL)
       ||
       (CU_add_test(suite1, "test_alloc_map_sets()", test_alloc_map_sets) == NULL)
       ||
       (CU_add_test(suite1, "test_alloc_map_sets_edge()", test_alloc_map_sets_edge) == NULL)
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
