#include <stdio.h>
#include <stdlib.h>
#include "gc.h"

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

void 
test_h_init ()
{
  CU_ASSERT(h_init(16, true, 1) != NULL); 
}

int
main (int argc, char *argv[])
{
  CU_pSuite suite1 = NULL;

  if (CU_initialize_registry() != CUE_SUCCESS)
    {
      return CU_get_error();
    }

  suite1 = CU_add_suite("h_init_test", NULL, NULL);

  if ((CU_add_test(suite1, "test_h_init()", test_h_init) == NULL))
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return CU_get_error();
}
