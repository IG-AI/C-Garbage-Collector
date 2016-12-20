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
  CU_ASSERT(size == INVALID);
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
  CU_ASSERT(size == INVALID);
}

void
test_get_data_size_upper_bound()
{
  size_t size = get_data_size(SIZE_MAX - HEADER_SIZE + 1);
  CU_ASSERT(size == INVALID);
}


/*============================================================================
 *                             TESTS FOR get_struct_size
 *===========================================================================*/

// ********************* Basic building Units ******************  //
void
test_get_struct_size_null()
{
  size_t size = get_struct_size(NULL);
  CU_ASSERT(size == INVALID);
}

void
test_get_struct_size_empty_string()
{
  size_t size = get_struct_size("");
  CU_ASSERT(size == INVALID);
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
  CU_ASSERT(size == INVALID);
}

void
test_get_struct_size_zero_ptr()
{
  size_t size = get_struct_size("0*");
  CU_ASSERT(size == INVALID);
}

void
test_get_struct_size_zero_before_valid_num()
{
  size_t size = get_struct_size("01*");
  CU_ASSERT(size == INVALID);
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

void
test_get_struct_size_hundred_ptr()
{
  size_t size = get_struct_size("100*");
  CU_ASSERT(size == 100 * PTR_SIZE + HEADER_SIZE);
}

// ********************* Invalid ******************  //
void
test_get_struct_size_invalid()
{
  size_t size = get_struct_size("->invalid");
  CU_ASSERT(size == INVALID);
}

void
test_get_struct_size_begin_num_invalid()
{
  size_t size = get_struct_size("3*invalid");
  CU_ASSERT(size == INVALID);
}

void
test_get_struct_size_invalid_end_num()
{
  size_t size = get_struct_size("invalid 3");
  CU_ASSERT(size == INVALID);
}

void
test_get_struct_size_too_big()
{
  size_t size = get_struct_size("5000000000000000000000000000001*");
  CU_ASSERT(size == INVALID);
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
 *                             TESTS FOR create_data_header
 *===========================================================================*/
void
test_create_data_header_zero_bytes()
{
  void *ptr = calloc(1, HEADER_SIZE);
  void *result = create_data_header(0, ptr);
  CU_ASSERT(result == NULL);
  free(ptr);
}

void
test_create_data_header_one_byte()
{
  void *ptr = calloc(1, HEADER_SIZE + 1);
  void *result = create_data_header(1, ptr);
  CU_ASSERT(result != NULL);
  CU_ASSERT((size_t) result - (size_t) ptr == HEADER_SIZE);
  CU_ASSERT(RAW_DATA == get_header_type(result));
  CU_ASSERT(get_existing_size(result) == 1 + HEADER_SIZE);
  free(ptr);
}

void
test_create_data_header_int()
{
  void *ptr = calloc(1, HEADER_SIZE + sizeof(int));
  void *result = create_data_header(sizeof(int), ptr);
  CU_ASSERT(result != NULL);
  CU_ASSERT((size_t) result - (size_t) ptr == HEADER_SIZE);
  CU_ASSERT(RAW_DATA == get_header_type(result));
  CU_ASSERT(get_existing_size(result) == sizeof(int) + HEADER_SIZE);
  free(ptr);
}

void
test_create_data_header_null_ptr()
{
  void *result = create_data_header(1, NULL);
  CU_ASSERT(result == NULL);
}


/*============================================================================
 *                             TESTS FOR create_struct_header
 *===========================================================================*/
void
test_create_struct_header_null_str()
{
  void *ptr = calloc(1, HEADER_SIZE);
  void *result = create_struct_header(NULL, ptr);
  CU_ASSERT(result == NULL);
  free(ptr);
}

void
test_create_struct_header_empty_str()
{
  void *ptr = calloc(1, HEADER_SIZE);
  void *result = create_struct_header("", ptr);
  CU_ASSERT(result == NULL);
  free(ptr);
}

void
test_create_struct_header_null_ptr()
{
  void *result = create_struct_header("*", NULL);
  CU_ASSERT(result == NULL);
}

void
test_create_struct_header_single_ptr()
{
  void *ptr = calloc(1, get_struct_size("*"));
  void *result = create_struct_header("*", ptr);
  CU_ASSERT(result != NULL);
  CU_ASSERT((size_t) result - (size_t) ptr == HEADER_SIZE);
  CU_ASSERT(STRUCT_REP == get_header_type(result));
  CU_ASSERT(get_existing_size(result) == get_struct_size("*"));
  free(ptr);
}

void
test_create_struct_header_complex()
{
  void *ptr = calloc(1, get_struct_size("3*2i3l"));
  void *result = create_struct_header("3*2i3l", ptr);
  CU_ASSERT(result != NULL);
  CU_ASSERT((size_t) result - (size_t) ptr == HEADER_SIZE);
  CU_ASSERT(STRUCT_REP == get_header_type(result));
  CU_ASSERT(get_existing_size(result) == get_struct_size("3*2i3l"));
  free(ptr);
}

void
test_create_struct_header_too_big_size()
{
  void *ptr = calloc(1, HEADER_SIZE);
  void *result = create_struct_header("5000000000000000000000000000001*", ptr);
  CU_ASSERT(result == NULL);
  free(ptr);
}

/*============================================================================
 *                             TESTS FOR get_header_type
 *===========================================================================*/
void
test_get_header_type_null()
{
  header_type t = get_header_type(NULL);
  CU_ASSERT(t == NOTHING);
}

void
test_get_header_type_data()
{
  void *ptr = calloc(1, get_data_size(sizeof(int) ) );
  void *result = create_data_header(sizeof(int), ptr);
  CU_ASSERT(RAW_DATA == get_header_type(result));
  free(ptr);
}

void
test_get_header_type_struct()
{
  void *ptr = calloc(1, get_struct_size("*") );
  void *result = create_struct_header("*", ptr);
  CU_ASSERT(STRUCT_REP == get_header_type(result));
  free(ptr);
}

/*
void
test_get_header_type_forward()
{
  void *ptr = calloc(1, get_struct_size("*") );
  void *result = create_struct_header("*", ptr);
  // Someting with forwarding
  CU_ASSERT(FORWARDING_ADDR == get_header_type(result));
  free(ptr);
}
*/


/*============================================================================
 *                  TESTS FOR get_number_of_pointers_in_structure
 *===========================================================================*/
void
test_get_number_pointers_null_ptr()
{
  int result = get_number_of_pointers_in_struct(NULL);
  CU_ASSERT(result == -1);
}

void
test_get_number_pointers_raw_data()
{
  void *ptr = calloc(1, get_data_size(sizeof(int) ) );
  void *data = create_data_header(sizeof(int), ptr);
  int result = get_number_of_pointers_in_struct(data);
  CU_ASSERT(result == -1);
  free(ptr);
}

/*
void
test_get_number_pointers_forwarding_data()
{
  void *ptr = calloc(1, get_data_size(sizeof(int) ) );
  void *data = create_data_header(sizeof(int), ptr);
  //Forwarding
  int result = get_number_of_pointers_in_struct(data);
  CU_ASSERT(result == -1);
  free(ptr);
  }*/

void
test_get_number_pointers_struct_no_ptr()
{
  void *ptr = calloc(1, get_struct_size("i"));
  void *data = create_struct_header("i", ptr);
  int result = get_number_of_pointers_in_struct(data);
  CU_ASSERT(result == 1);
  free(ptr);
}

void
test_get_number_pointers_struct_one_ptr()
{
  void *ptr = calloc(1, get_struct_size("*"));
  void *data = create_struct_header("*", ptr);
  int result = get_number_of_pointers_in_struct(data);
  CU_ASSERT(result == 2);
  free(ptr);
}

void
test_get_number_pointers_struct_multi_ptr()
{
  void *ptr = calloc(1, get_struct_size("***"));
  void *data = create_struct_header("***", ptr);
  int result = get_number_of_pointers_in_struct(data);
  CU_ASSERT(result == 4);
  free(ptr);
}


void
test_get_number_pointers_struct_multi_ptr_nr()
{
  void *ptr = calloc(1, get_struct_size("3*"));
  void *data = create_struct_header("3*", ptr);
  int result = get_number_of_pointers_in_struct(data);
  CU_ASSERT(result == 4);
  free(ptr);
}

void
test_get_number_pointers_struct_mixed_ptr()
{
  void *ptr = calloc(1, get_struct_size("*i3*"));
  void *data = create_struct_header("*i3*", ptr);
  int result = get_number_of_pointers_in_struct(data);
  CU_ASSERT(result == 5);
  free(ptr);
}


/*============================================================================
 *                             TESTS FOR get_pointers_in_structure
 *===========================================================================*/
void
test_get_pointers_null_ptr()
{
  void **array[1];
  bool result = get_pointers_in_struct(NULL, array);
  CU_ASSERT_FALSE(result);
}

void
test_get_pointers_null_array()
{
  void *ptr = calloc(1, get_struct_size("*"));
  void *data = create_struct_header("*", ptr);
  bool result = get_pointers_in_struct(data, NULL);
  CU_ASSERT_FALSE(result);
  free(ptr);
}

void
test_get_pointers_raw_data()
{
  void **array[1];
  void *ptr = calloc(1, get_data_size(sizeof(int) ) );
  void *data = create_data_header(sizeof(int), ptr);
  bool result = get_pointers_in_struct(data, array);
  CU_ASSERT_FALSE(result);
  free(ptr);
}

/*
void
test_get_pointers_forwarding_data()
{
  void **array[1];
  void *ptr = calloc(1, get_data_size(sizeof(int) ) );
  void *data = create_data_header(sizeof(int), ptr);
  //Forwarding
  bool result = get_pointers_in_struct(data, array);
  CU_ASSERT_FALSE(result);
  free(ptr);
  }*/

void
test_get_pointers_struct_no_ptr()
{
  void **array[1];
  void *ptr = calloc(1, get_struct_size("i"));
  void *data = create_struct_header("i", ptr);
  bool result = get_pointers_in_struct(data, array);
  CU_ASSERT_TRUE(result);
  CU_ASSERT(array[0] != NULL);
  CU_ASSERT(strcmp((char *) array[0], "i")); // Change with Bitvector
  free(ptr);
}

void
test_get_pointers_struct_one_ptr()
{
  void *ptr = calloc(1, get_struct_size("*"));
  void *data = create_struct_header("*", ptr);
  *(void **)data = (void *) 1UL;
  int size = get_number_of_pointers_in_struct(data);
  void **array[size];
  for(int i = 0; i < size; ++i) array[i] = NULL;
  
  bool result = get_pointers_in_struct(data, array);
  CU_ASSERT_TRUE(result);
  for(int i = 0; i < size; ++i)
    {
      CU_ASSERT(array[i] != NULL);
    }
  CU_ASSERT(*(unsigned long*)array[1] == 1UL);
  free(ptr);
}

void
test_get_pointers_struct_multi_ptr()
{
  void *ptr = calloc(1, get_struct_size("***"));
  void *data = create_struct_header("***", ptr);
  for(unsigned long i = 0; i < 3; ++i)
    {
      ((unsigned long *)data)[i] = i+1;
    }
  int size = get_number_of_pointers_in_struct(data);
  void **array[size];
  for(int i = 0; i < size; ++i) array[i] = NULL;
  
  bool result = get_pointers_in_struct(data, array);
  CU_ASSERT_TRUE(result);
  CU_ASSERT(array[0] != NULL);
  for(unsigned long i = 1; i < 4; ++i)
    {
      CU_ASSERT(*(unsigned long *)array[i] == i);
    }
  free(ptr);
}


void
test_get_pointers_struct_multi_ptr_nr()
{
  void *ptr = calloc(1, get_struct_size("3*"));
  void *data = create_struct_header("3*", ptr);
  for(unsigned long i = 0; i < 3; ++i)
    {
      ((unsigned long *)data)[i] = i+1;
    }
  int size = get_number_of_pointers_in_struct(data);
  void **array[size];
  for(int i = 0; i < size; ++i) array[i] = NULL;
  
  bool result = get_pointers_in_struct(data, array);
  CU_ASSERT_TRUE(result);
  CU_ASSERT(array[0] != NULL);
  for(unsigned long i = 1; i < 4; ++i)
    {
      CU_ASSERT(*(unsigned long *)array[i] == i);
    }
  free(ptr);
}

void
test_get_pointers_struct_mixed_ptr()
{
  void *ptr = calloc(1, get_struct_size("*i3*"));
  void *data = create_struct_header("*i3*", ptr);
  int size = get_number_of_pointers_in_struct(data);
  void **array[size];
  for(int i = 0; i < size; ++i) array[i] = NULL;
  
  bool result = get_pointers_in_struct(data, array);
  CU_ASSERT_TRUE(result);
  for(int i = 0; i < size; ++i)
    {
      CU_ASSERT(array[i] != NULL);
    }
  free(ptr);
}


/*============================================================================
 *                             TESTS FOR get_existing_size
 *===========================================================================*/

void
test_get_existing_size_null_ptr()
{
  size_t result = get_existing_size(NULL);
  CU_ASSERT(result == INVALID);
}

/*
void
test_get_existing_size_forwarding()
{
  void *ptr = calloc(1, get_struct_size("*"));
  void *data = create_struct_header("*", ptr);
  //Forwarding
  size_t result = get_existing_size(data);
  CU_ASSERT(result == INVALID);
  free(ptr);
}
*/


void
test_get_existing_size_raw_data_eigth()
{
  void *ptr = calloc(1, get_data_size(8));
  void *data = create_data_header(8, ptr);
  size_t result = get_existing_size(data);
  CU_ASSERT(result == get_data_size(8));
  free(ptr);
}

void
test_get_existing_size_raw_data_one()
{
  void *ptr = calloc(1, get_data_size(1));
  void *data = create_data_header(1, ptr);
  size_t result = get_existing_size(data);
  CU_ASSERT(result == get_data_size(1));
  free(ptr);
}


void
test_get_existing_size_struct_rep_one_ptr()
{
  void *ptr = calloc(1, get_struct_size("*"));
  void *data = create_struct_header("*", ptr);
  size_t result = get_existing_size(data);
  CU_ASSERT(result == get_struct_size("*"));
  free(ptr);
}

void
test_get_existing_size_struct_rep_char()
{
  void *ptr = calloc(1, get_struct_size("c"));
  void *data = create_struct_header("c", ptr);
  size_t result = get_existing_size(data);
  CU_ASSERT(result == get_struct_size("c"));
  free(ptr);
}

void
test_get_existing_size_struct_rep_int()
{
  void *ptr = calloc(1, get_struct_size("i"));
  void *data = create_struct_header("i", ptr);
  size_t result = get_existing_size(data);
  CU_ASSERT(result == get_struct_size("i"));
  free(ptr);
}

void
test_get_existing_size_struct_rep_long()
{
  void *ptr = calloc(1, get_struct_size("l"));
  void *data = create_struct_header("l", ptr);
  size_t result = get_existing_size(data);
  CU_ASSERT(result == get_struct_size("l"));
  free(ptr);
}

void
test_get_existing_size_struct_rep_float()
{
  void *ptr = calloc(1, get_struct_size("f"));
  void *data = create_struct_header("f", ptr);
  size_t result = get_existing_size(data);
  CU_ASSERT(result == get_struct_size("f"));
  free(ptr);
}

void
test_get_existing_size_struct_rep_double()
{
  void *ptr = calloc(1, get_struct_size("d"));
  void *data = create_struct_header("d", ptr);
  size_t result = get_existing_size(data);
  CU_ASSERT(result == get_struct_size("d"));
  free(ptr);
}

// ********************* Combinations ******************  //
void
test_get_existing_size_ptr_int()
{
  void *ptr = calloc(1, get_struct_size("*i"));
  void *data = create_struct_header("*i", ptr);
  size_t result = get_existing_size(data);
  CU_ASSERT(result == get_struct_size("*i"));
  free(ptr);
}

void
test_get_existing_size_ptr_ptr()
{
  void *ptr = calloc(1, get_struct_size("**"));
  void *data = create_struct_header("**", ptr);
  size_t result = get_existing_size(data);
  CU_ASSERT(result == get_struct_size("**"));
  free(ptr);
}

void
test_get_existing_size_ptr_char_ptr()
{
  void *ptr = calloc(1, get_struct_size("*c*"));
  void *data = create_struct_header("*c*", ptr);
  size_t result = get_existing_size(data);
  CU_ASSERT(result == get_struct_size("*c*"));
  free(ptr);
}


// ********************* Basic Numerical ******************  //
void
test_get_existing_size_one_ptr()
{
  void *ptr = calloc(1, get_struct_size("1*"));
  void *data = create_struct_header("1*", ptr);
  size_t result = get_existing_size(data);
  CU_ASSERT(result == get_struct_size("1*"));
  free(ptr);
}

void
test_get_existing_size_one()
{
  void *ptr = calloc(1, get_struct_size("1"));
  void *data = create_struct_header("1", ptr);
  size_t result = get_existing_size(data);
  CU_ASSERT(result == get_struct_size("1"));
  free(ptr);
}

void
test_get_existing_size_twelve_ptr()
{
  void *ptr = calloc(1, get_struct_size("12*"));
  void *data = create_struct_header("12*", ptr);
  size_t result = get_existing_size(data);
  CU_ASSERT(result == get_struct_size("12*"));
  free(ptr);
}

void
test_get_existing_size_hundred_twelve_ptr()
{
  void *ptr = calloc(1, get_struct_size("112*"));
  void *data = create_struct_header("112*", ptr);
  size_t result = get_existing_size(data);
  CU_ASSERT(result == get_struct_size("112*"));
  free(ptr);
}

void
test_get_existing_size_hundred_ptr()
{
  void *ptr = calloc(1, get_struct_size("100*"));
  void *data = create_struct_header("100*", ptr);
  size_t result = get_existing_size(data);
  CU_ASSERT(result == get_struct_size("100*"));
  free(ptr);
}

// ********************* Numerical Combinations ******************  //

void
test_get_existing_size_two_ptr_three_int()
{
  void *ptr = calloc(1, get_struct_size("2*3i"));
  void *data = create_struct_header("2*3i", ptr);
  size_t result = get_existing_size(data);
  CU_ASSERT(result == get_struct_size("2*3i"));
  free(ptr);
}

void
test_get_existing_size_two_ptr_ptr()
{
  void *ptr = calloc(1, get_struct_size("2**"));
  void *data = create_struct_header("2**", ptr);
  size_t result = get_existing_size(data);
  CU_ASSERT(result == get_struct_size("2**"));
  free(ptr);
}

void
test_get_existing_size_num_after_letter()
{
  void *ptr = calloc(1, get_struct_size("d2"));
  void *data = create_struct_header("d2", ptr);
  size_t result = get_existing_size(data);
  CU_ASSERT(result == get_struct_size("d2"));
  free(ptr);
}

/*============================================================================
 *                             MAIN TESTING UNIT
 *===========================================================================*/
int
main(void)
{
  CU_pSuite suite_data_size = NULL;
  CU_pSuite suite_struct_size = NULL;
  CU_pSuite suite_create_data_header = NULL;
  CU_pSuite suite_create_struct_header = NULL;
  CU_pSuite suite_get_header_type = NULL;
  CU_pSuite suite_get_num_ptr = NULL;
  CU_pSuite suite_get_ptrs = NULL;
  CU_pSuite suite_existing_size = NULL;


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
                               , "Case zero before valid num"
                               , test_get_struct_size_zero_before_valid_num) )
       || (NULL == CU_add_test(suite_struct_size
                               , "Case 12*"
                               , test_get_struct_size_twelve_ptr) )
       || (NULL == CU_add_test(suite_struct_size
                               , "Case 112*"
                               , test_get_struct_size_hundred_twelve_ptr) )
       || (NULL == CU_add_test(suite_struct_size
                               , "Case zero inside number"
                               , test_get_struct_size_hundred_ptr) )
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


    
  // ********************* create_data_header SUITE ******************  //
  suite_create_data_header = CU_add_suite("Tests function create_data_header()"
                                 , NULL, NULL);
  if (suite_create_data_header == NULL) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if ( (NULL == CU_add_test(suite_create_data_header
                            , "Case bytes is 0"
                            , test_create_data_header_zero_bytes) )
       || (NULL == CU_add_test(suite_create_data_header
                               , "Case bytes is 1)"
                               , test_create_data_header_one_byte) )
       || (NULL == CU_add_test(suite_create_data_header
                               , "Case bytes is sizeof(int)"
                               , test_create_data_header_int) )
       || (NULL == CU_add_test(suite_create_data_header
                               , "Case ptr is NULL"
                               , test_create_data_header_null_ptr) )
       )
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  // ********************* create_struct_header SUITE ******************  //
  suite_create_struct_header = CU_add_suite("Tests function create_struct_header()"
                                 , NULL, NULL);
  if (suite_create_struct_header == NULL) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if ( (NULL == CU_add_test(suite_create_struct_header
                            , "NULL format string"
                            , test_create_struct_header_null_str) )
       || (NULL == CU_add_test(suite_create_struct_header
                               , "NULL heap pointer"
                               , test_create_struct_header_null_ptr) )
       || (NULL == CU_add_test(suite_create_struct_header
                               , "Empty format string"
                               , test_create_struct_header_empty_str) )
       || (NULL == CU_add_test(suite_create_struct_header
                               , "Single \"*\""
                               , test_create_struct_header_single_ptr) )
       
       || (NULL == CU_add_test(suite_create_struct_header
                               , "Complex format string"
                               , test_create_struct_header_complex) )
       || (NULL == CU_add_test(suite_create_struct_header
                               , "Format string rep. too big size"
                               , test_create_struct_header_too_big_size) )
       )
    {
      CU_cleanup_registry();
      return CU_get_error();
    }
  
  // ********************* get_header_type SUITE ******************  //
  suite_get_header_type = CU_add_suite("Tests function get_header_type()"
                                 , NULL, NULL);
  if (suite_get_header_type == NULL) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if ( (NULL == CU_add_test(suite_get_header_type
                            , "Null pointer"
                            , test_get_header_type_null) )
       || (NULL == CU_add_test(suite_get_header_type
                               , "Data header"
                               , test_get_header_type_data) )
       || (NULL == CU_add_test(suite_get_header_type
                               , "Struct header"
                               , test_get_header_type_struct) )
       /*|| (NULL == CU_add_test(suite_get_header_type
                               , "Forwarded header"
                               , test_get_header_type_forward) )
       */)
    {
      CU_cleanup_registry();
      return CU_get_error();
    }


   
  // ********************* get_num_ptr SUITE ******************  //
  suite_get_num_ptr = CU_add_suite("Tests function get_number_of"
                                   "_pointers_in_struct()"
                                 , NULL, NULL);
  if (suite_get_num_ptr == NULL) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if ( (NULL == CU_add_test(suite_get_num_ptr
                            , "Null ptr"
                            , test_get_number_pointers_null_ptr) )
       || (NULL == CU_add_test(suite_get_num_ptr
                               , "Raw data"
                               , test_get_number_pointers_raw_data) )
       /*  || (NULL == CU_add_test(suite_get_num_ptr
                               , "Forwarding"
                               , test_get_number_pointers_forwarding_data) )*/
       || (NULL == CU_add_test(suite_get_num_ptr
                               , "No ptrs"
                               , test_get_number_pointers_struct_no_ptr) )
       || (NULL == CU_add_test(suite_get_num_ptr
                               , "One ptr"
                               , test_get_number_pointers_struct_one_ptr) )
       || (NULL == CU_add_test(suite_get_num_ptr
                               , "Multi ptrs"
                               , test_get_number_pointers_struct_multi_ptr) )
       || (NULL == CU_add_test(suite_get_num_ptr
                               , "Multi ptrs with nr"
                               , test_get_number_pointers_struct_multi_ptr_nr) )
       || (NULL == CU_add_test(suite_get_num_ptr
                               , "Mixed ptrs"
                               , test_get_number_pointers_struct_mixed_ptr) )
       
    )
    {
      CU_cleanup_registry();
      return CU_get_error();
    }


  // ********************* get_ptrs SUITE ******************  //
  suite_get_ptrs = CU_add_suite("Tests function get_pointers_in_struct()"
                                   , NULL, NULL);
  if (suite_get_ptrs == NULL) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if ( (NULL == CU_add_test(suite_get_ptrs
                            , "Null ptr"
                            , test_get_pointers_null_ptr) )
       || (NULL == CU_add_test(suite_get_ptrs
                               , "Null array"
                               , test_get_pointers_null_array) )
       || (NULL == CU_add_test(suite_get_ptrs
                               , "Raw data"
                               , test_get_pointers_raw_data) )
       /*  || (NULL == CU_add_test(suite_get_ptrs
           , "Forwarding"
           , test_get_pointers_forwarding_data) )*/
       || (NULL == CU_add_test(suite_get_ptrs
                               , "No ptrs"
                               , test_get_pointers_struct_no_ptr) )
       || (NULL == CU_add_test(suite_get_ptrs
                               , "One ptr"
                               , test_get_pointers_struct_one_ptr) )
       || (NULL == CU_add_test(suite_get_ptrs
                               , "Multi ptrs"
                               , test_get_pointers_struct_multi_ptr) )
       || (NULL == CU_add_test(suite_get_ptrs
                               , "Multi ptrs with nr"
                               , test_get_pointers_struct_multi_ptr_nr) )
       || (NULL == CU_add_test(suite_get_ptrs
                               , "Mixed ptrs"
                               , test_get_pointers_struct_mixed_ptr) )
       
       )
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  // ********************* get_existing_size SUITE ******************  //
  suite_existing_size = CU_add_suite("Tests function get_existing_size()"
                                   , NULL, NULL);
  if (suite_existing_size == NULL) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if ( (NULL == CU_add_test(suite_existing_size
                            , "Null ptr"
                            , test_get_existing_size_null_ptr) )
       /*       || (NULL == CU_add_test(suite_existing_size
                               , "forwarding"
                               , test_get_existing_size_forwarding) )*/
       || (NULL == CU_add_test(suite_existing_size
                               , "Raw data 8"
                               , test_get_existing_size_raw_data_eigth) )
       || (NULL == CU_add_test(suite_existing_size
                               , "Raw data 1"
                               , test_get_existing_size_raw_data_one) )
       || (NULL == CU_add_test(suite_existing_size
                               , "Case *"
                               , test_get_existing_size_struct_rep_one_ptr) )
       || (NULL == CU_add_test(suite_existing_size
                               , "Case char"
                               , test_get_existing_size_struct_rep_char) )
       || (NULL == CU_add_test(suite_existing_size
                               , "Case int"
                               , test_get_existing_size_struct_rep_int) )
       || (NULL == CU_add_test(suite_existing_size
                               , "Case long"
                               , test_get_existing_size_struct_rep_long) )
       || (NULL == CU_add_test(suite_existing_size
                               , "Case float"
                               , test_get_existing_size_struct_rep_float) )
       || (NULL == CU_add_test(suite_existing_size
                               , "Case double"
                               , test_get_existing_size_struct_rep_double) )
       || (NULL == CU_add_test(suite_existing_size
                               , "Case *i"
                               , test_get_existing_size_ptr_int) )
       || (NULL == CU_add_test(suite_existing_size
                               , "Case **"
                               , test_get_existing_size_ptr_ptr) )
       || (NULL == CU_add_test(suite_existing_size
                               , "Case *c*"
                               , test_get_existing_size_ptr_char_ptr) )
       || (NULL == CU_add_test(suite_existing_size
                               , "Case 1*"
                               , test_get_existing_size_one_ptr) )
       || (NULL == CU_add_test(suite_existing_size
                               , "Case 1"
                               , test_get_existing_size_one) )
       || (NULL == CU_add_test(suite_existing_size
                               , "Case 12*"
                               , test_get_existing_size_twelve_ptr) )
       || (NULL == CU_add_test(suite_existing_size
                               , "Case 112*"
                               , test_get_existing_size_hundred_twelve_ptr) )
       || (NULL == CU_add_test(suite_existing_size
                               , "Case zero inside number"
                               , test_get_existing_size_hundred_ptr) )
       || (NULL == CU_add_test(suite_existing_size
                               , "Case 2*3i"
                               , test_get_existing_size_two_ptr_three_int) )
       || (NULL == CU_add_test(suite_existing_size
                               , "Case 2**"
                               , test_get_existing_size_two_ptr_ptr) )
       || (NULL == CU_add_test(suite_existing_size
                               , "Case d2"
                               , test_get_existing_size_num_after_letter) )
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
