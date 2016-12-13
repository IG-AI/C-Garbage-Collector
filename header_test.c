#include <stdint.h>
#include "CUnit/Basic.h"
#include "header.h"
#include "header_hidden.h"




/*============================================================================
 *                             TESTS FOR get_data_size
 *===========================================================================*/
void
test_get_data_size_zero()
{
  size_t size = get_data_size(0);
  CU_ASSERT(size == 0);
}

void
test_get_data_size_one()
{
  size_t size = get_data_size(1);
  CU_ASSERT(size == 1 + HEADER_SIZE);
}

void
test_get_data_size_int()
{
  size_t size = get_data_size(sizeof(int) );
  CU_ASSERT(size == sizeof(int) + HEADER_SIZE);
}

void
test_get_data_size_long()
{
  size_t size = get_data_size(sizeof(long) );
  CU_ASSERT(size == sizeof(long) + HEADER_SIZE);
}

void
test_get_data_size_max()
{
  size_t size = get_data_size(SIZE_MAX);
  CU_ASSERT(size == 0);
}

void
test_get_data_size_upper_bound()
{
  size_t size = get_data_size(SIZE_MAX - HEADER_SIZE + 1);
  CU_ASSERT(size == 0);
}


/*============================================================================
 *                             MAIN TESTING UNIT
 *===========================================================================*/
int
main(void)
{
  CU_pSuite suite_data_size = NULL;

  if (CU_initialize_registry() != CUE_SUCCESS)
    {
      return CU_get_error();
    }

  
  // ********************* get_data_size SUITE ******************  //
  suite_data_size = CU_add_suite("Tests function get_data_size()", NULL, NULL);
  if (suite_data_size == NULL) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if ( (NULL == CU_add_test(suite_data_size, "Case get_data_size(0)"
                            , test_get_data_size_zero) )
       || (NULL == CU_add_test(suite_data_size
                               , "Case get_data_size(1)"
                               , test_get_data_size_one) )
       || (NULL == CU_add_test(suite_data_size
                               , "Case get_data_size(sizeof(int))"
                               , test_get_data_size_int) )
       || (NULL == CU_add_test(suite_data_size
                               , "Case get_data_size(sizeof(long))"
                               , test_get_data_size_long) )
       || (NULL == CU_add_test(suite_data_size
                               , "Case get_data_size(), max size_t"
                               , test_get_data_size_max) )
       || (NULL == CU_add_test(suite_data_size
                               , "Case get_data_size(), upperbound"
                               , test_get_data_size_upper_bound) )
       
    )
    {
      CU_cleanup_registry();
      return CU_get_error();
    }


  // ******************** RUN TESTS ***************** //
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();
  return CU_get_error();
}
