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
 *                             TESTS FOR get_struct_size
 *===========================================================================*/

// ********************* Basic building Units ******************  //
void
test_get_struct_size_null()
{
  size_t size = get_struct_size(NULL);
  CU_ASSERT(size == 0);
}

void
test_get_struct_size_empty_string()
{
  size_t size = get_struct_size("");
  CU_ASSERT(size == 0);
}

void
test_get_struct_size_ptr()
{
  size_t size = get_struct_size("*");
  CU_ASSERT(size == PTR_SIZE + HEADER_SIZE);
}

void
test_get_struct_size_char()
{
  size_t size = get_struct_size("c");
  CU_ASSERT(size == CHAR_SIZE + HEADER_SIZE);
}

void
test_get_struct_size_int()
{
  size_t size = get_struct_size("i");
  CU_ASSERT(size == INT_SIZE + HEADER_SIZE);
}

void
test_get_struct_size_long()
{
  size_t size = get_struct_size("l");
  CU_ASSERT(size == LONG_SIZE + HEADER_SIZE);
}

void
test_get_struct_size_float()
{
  size_t size = get_struct_size("f");
  CU_ASSERT(size == FLOAT_SIZE + HEADER_SIZE);
}

void
test_get_struct_size_double()
{
  size_t size = get_struct_size("d");
  CU_ASSERT(size == DOUBLE_SIZE + HEADER_SIZE);
}


// ********************* Combinations ******************  //
void
test_get_struct_size_ptr_int()
{
  size_t size = get_struct_size("*i");
  CU_ASSERT(size == PTR_SIZE + INT_SIZE + HEADER_SIZE);
}

void
test_get_struct_size_ptr_ptr()
{
  size_t size = get_struct_size("**");
  CU_ASSERT(size == PTR_SIZE + PTR_SIZE + HEADER_SIZE);
}

void
test_get_struct_size_ptr_char_ptr()
{
  size_t size = get_struct_size("*c*");
  CU_ASSERT(size == PTR_SIZE + CHAR_SIZE + PTR_SIZE + HEADER_SIZE);
}


// ********************* Basic Numerical ******************  //
void
test_get_struct_size_one_ptr()
{
  size_t size = get_struct_size("1*");
  CU_ASSERT(size == PTR_SIZE + HEADER_SIZE);
}

void
test_get_struct_size_one_int()
{
  size_t size = get_struct_size("1i");
  CU_ASSERT(size == INT_SIZE + HEADER_SIZE);
}

void
test_get_struct_size_one()
{
  size_t size = get_struct_size("1");
  CU_ASSERT(size == CHAR_SIZE + HEADER_SIZE);
}

void
test_get_struct_size_two_ptr()
{
  size_t size = get_struct_size("2*");
  CU_ASSERT(size == 2 * PTR_SIZE + HEADER_SIZE);
}

void
test_get_struct_size_zero()
{
  size_t size = get_struct_size("0");
  CU_ASSERT(size == 0);
}

void
test_get_struct_size_zero_ptr()
{
  size_t size = get_struct_size("0*");
  CU_ASSERT(size == 0);
}

void
test_get_struct_size_twelve_ptr()
{
  size_t size = get_struct_size("12*");
  CU_ASSERT(size == 12 * PTR_SIZE + HEADER_SIZE);
}

void
test_get_struct_size_hundred_twelve_ptr()
{
  size_t size = get_struct_size("112*");
  CU_ASSERT(size == 112 * PTR_SIZE + HEADER_SIZE);
}

// ********************* Invalid ******************  //
void
test_get_struct_size_invalid()
{
  size_t size = get_struct_size("->invalid");
  CU_ASSERT(size == 0);
}

void
test_get_struct_size_begin_num_invalid()
{
  size_t size = get_struct_size("3*invalid");
  CU_ASSERT(size == 0);
}

void
test_get_struct_size_invalid_end_num()
{
  size_t size = get_struct_size("invalid 3");
  CU_ASSERT(size == 0);
}

void
test_get_struct_size_too_big()
{
  size_t size = get_struct_size("5000000000000000000000000000000*");
  CU_ASSERT(size == 0);
}

// ********************* Numerical Combinations ******************  //
void
test_get_struct_size_two_ptr_int()
{
  size_t size = get_struct_size("2*i");
  CU_ASSERT(size == 2 * PTR_SIZE + INT_SIZE + HEADER_SIZE);
}

void
test_get_struct_size_two_ptr_three_int()
{
  size_t size = get_struct_size("2*3i");
  CU_ASSERT(size == 2 * PTR_SIZE + 3 * INT_SIZE + HEADER_SIZE);
}

void
test_get_struct_size_two_ptr_ptr()
{
  size_t size = get_struct_size("2**");
  CU_ASSERT(size == 2 * PTR_SIZE + PTR_SIZE + HEADER_SIZE);
}

void
test_get_struct_size_double_two_int()
{
  size_t size = get_struct_size("2i2i");
  CU_ASSERT(size == 2 * INT_SIZE + 2 * INT_SIZE + HEADER_SIZE);
}

void
test_get_struct_size_num_after_letter()
{
  size_t size = get_struct_size("d2");
  CU_ASSERT(size == DOUBLE_SIZE + 2 * CHAR_SIZE + HEADER_SIZE);
}

/*============================================================================
 *                             MAIN TESTING UNIT
 *===========================================================================*/
int
main(void)
{
  CU_pSuite suite_data_size = NULL;
  CU_pSuite suite_struct_size = NULL;

  if (CU_initialize_registry() != CUE_SUCCESS)
    {
      return CU_get_error();
    }

  
  // ********************* get_data_size SUITE ******************  //
  suite_data_size = CU_add_suite("Tests function get_data_size()"
                                 , NULL, NULL);
  if (suite_data_size == NULL) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if ( (NULL == CU_add_test(suite_data_size
                            , "Case get_data_size(0)"
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

  // ********************* get_struct_size SUITE ******************  //
  suite_struct_size = CU_add_suite("Tests function get_struct_size()"
                                   , NULL, NULL);
  if (suite_struct_size == NULL) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if ( (NULL == CU_add_test(suite_struct_size
                            , "Case NULL"
                            , test_get_struct_size_null) )
       || (NULL == CU_add_test(suite_struct_size
                               , "Case \"\""
                               , test_get_struct_size_empty_string) )
       || (NULL == CU_add_test(suite_struct_size
                               , "Case *"
                               , test_get_struct_size_ptr) )
       || (NULL == CU_add_test(suite_struct_size
                               , "Case char"
                               , test_get_struct_size_char) )
       || (NULL == CU_add_test(suite_struct_size
                               , "Case int"
                               , test_get_struct_size_int) )
       || (NULL == CU_add_test(suite_struct_size
                               , "Case long"
                               , test_get_struct_size_long) )
       || (NULL == CU_add_test(suite_struct_size
                               , "Case float"
                               , test_get_struct_size_float) )
       || (NULL == CU_add_test(suite_struct_size
                               , "Case double"
                               , test_get_struct_size_double) )
       || (NULL == CU_add_test(suite_struct_size
                               , "Case *i"
                               , test_get_struct_size_ptr_int) )
       || (NULL == CU_add_test(suite_struct_size
                               , "Case **"
                               , test_get_struct_size_ptr_ptr) )
       || (NULL == CU_add_test(suite_struct_size
                               , "Case *c*"
                               , test_get_struct_size_ptr_char_ptr) )
       || (NULL == CU_add_test(suite_struct_size
                               , "Case 1*"
                               , test_get_struct_size_one_ptr) )
       || (NULL == CU_add_test(suite_struct_size
                               , "Case 1i"
                               , test_get_struct_size_one_int) )
       || (NULL == CU_add_test(suite_struct_size
                               , "Case 1"
                               , test_get_struct_size_one) )
       || (NULL == CU_add_test(suite_struct_size
                               , "Case 2*"
                               , test_get_struct_size_two_ptr) )
       || (NULL == CU_add_test(suite_struct_size
                               , "Case 0"
                               , test_get_struct_size_zero) )
       || (NULL == CU_add_test(suite_struct_size
                               , "Case 0*"
                               , test_get_struct_size_zero_ptr) )
       || (NULL == CU_add_test(suite_struct_size
                               , "Case 12*"
                               , test_get_struct_size_twelve_ptr) )
       || (NULL == CU_add_test(suite_struct_size
                               , "Case 112*"
                               , test_get_struct_size_hundred_twelve_ptr) )
       || (NULL == CU_add_test(suite_struct_size
                               , "Case invalid"
                               , test_get_struct_size_invalid) )
       || (NULL == CU_add_test(suite_struct_size
                               , "Case num invalid"
                               , test_get_struct_size_begin_num_invalid) )
       || (NULL == CU_add_test(suite_struct_size
                               , "Case end invalid"
                               , test_get_struct_size_invalid_end_num) )
       || (NULL == CU_add_test(suite_struct_size
                               , "Case size too big"
                               , test_get_struct_size_too_big) )
       || (NULL == CU_add_test(suite_struct_size
                               , "Case 2*i"
                               , test_get_struct_size_two_ptr_int) )
       || (NULL == CU_add_test(suite_struct_size
                               , "Case 2*3i"
                               , test_get_struct_size_two_ptr_three_int) )
       || (NULL == CU_add_test(suite_struct_size
                               , "Case 2**"
                               , test_get_struct_size_two_ptr_ptr) )
       || (NULL == CU_add_test(suite_struct_size
                               , "Case 2i2i"
                               , test_get_struct_size_double_two_int) )
       || (NULL == CU_add_test(suite_struct_size
                               , "Case d2"
                               , test_get_struct_size_num_after_letter) )
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
