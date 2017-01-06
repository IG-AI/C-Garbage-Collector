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

struct test_struct
{
  int i;
  char c;
  void *ptr;
  long l;
  double d;
};

typedef struct test_struct test_struct_t;

#define TEST_STRUCT_FORMAT_STR "ic*ld"


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

void **
back_up_ptr(void *ptr)
{
  void **alloced_ptr = malloc(sizeof(void *));
  *alloced_ptr = ptr;
  return alloced_ptr;
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
  CU_ASSERT(h == NULL);
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
 *                      h_delete/h_delete_dbg TESTING SUITE
 *===========================================================================*/

void
test_h_delete_null_ptr()
{
  h_delete(NULL);
  CU_ASSERT_TRUE(true);
}

void
test_h_delete_dbg_null_ptr()
{
  h_delete_dbg(NULL, NULL);
  CU_ASSERT_TRUE(true);
}

void
test_h_delete_dbg_null_value()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  void *data_ptr = h_alloc_data(h, sizeof(int) );
  h_delete_dbg(h, NULL);
  CU_ASSERT(data_ptr == NULL);
}

void
test_h_delete_dbg_other_value()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  void *data_ptr = h_alloc_data(h, sizeof(int) );
  unsigned long dbg_value = 0xDEADBEEF;
  h_delete_dbg(h, (void *) dbg_value);
  CU_ASSERT(data_ptr == (void *) dbg_value);
}

void
test_h_delete_dbg_multiple_ptrs_to_same_data()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  void *first_ptr = h_alloc_data(h, sizeof(int) );
  void *second_ptr = first_ptr;
  h_delete_dbg(h, NULL);
  CU_ASSERT(first_ptr == NULL);
  CU_ASSERT(second_ptr == NULL);
}

void
test_h_delete_dbg_multiple_ptrs()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  void *first_ptr = h_alloc_data(h, sizeof(int) );
  void *second_ptr = h_alloc_data(h, sizeof(int) );
  h_delete_dbg(h, NULL);
  CU_ASSERT(first_ptr == NULL);
  CU_ASSERT(second_ptr == NULL);
}

void
test_h_delete_dbg_ptr_into_unused_heap_area()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  void *data_ptr = h_alloc_data(h, sizeof(int) );
  void *ptr_to_unused = (void *) ( (unsigned long) data_ptr + 30);
  h_delete_dbg(h, NULL);
  CU_ASSERT(data_ptr == NULL);
  CU_ASSERT(ptr_to_unused != NULL);
}

void
test_h_delete_dbg_struct()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  void *struct_ptr = h_alloc_struct(h, TEST_STRUCT_FORMAT_STR);
  h_delete_dbg(h, NULL);
  CU_ASSERT(struct_ptr == NULL);
}



/*============================================================================
 *                             h_alloc_struct TESTING SUITE
 *===========================================================================*/

void
test_h_alloc_struct_null_heap()
{
  void *struct_ptr = h_alloc_struct(NULL, TEST_STRUCT_FORMAT_STR);
  CU_ASSERT(struct_ptr == NULL);
}

void
test_h_alloc_struct_null_str()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  void *struct_ptr = h_alloc_struct(h, NULL);
  CU_ASSERT(struct_ptr == NULL);
  h_delete(h);
}

void
test_h_alloc_struct_empty_str()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  void *struct_ptr = h_alloc_struct(h, "");
  CU_ASSERT(struct_ptr == NULL);
  h_delete(h);
}

void
test_h_alloc_struct_invalid_str()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  size_t avail_before = h_avail(h);
  size_t used_before = h_used(h);
  void *struct_ptr = h_alloc_struct(h, "asd");
  CU_ASSERT(struct_ptr == NULL);
  size_t avail_after = h_avail(h);
  size_t used_after = h_used(h);
  CU_ASSERT(avail_before == avail_after);
  CU_ASSERT(used_before == used_after);
  h_delete(h);
}

void
test_h_alloc_struct_valid_str()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  size_t avail_before = h_avail(h);
  size_t used_before = h_used(h);
  void *struct_ptr = h_alloc_struct(h, TEST_STRUCT_FORMAT_STR);
  CU_ASSERT(struct_ptr != NULL);
  size_t avail_after = h_avail(h);
  size_t used_after = h_used(h);
  CU_ASSERT(avail_before > avail_after);
  CU_ASSERT(used_before < used_after);
  *(test_struct_t *)struct_ptr = (test_struct_t){1, 'a', NULL, 2, 1.0};
  h_delete(h);
}

void
test_h_alloc_struct_too_big_for_page()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  size_t avail_before = h_avail(h);
  size_t used_before = h_used(h);
  void *struct_ptr = h_alloc_struct(h, "2049c");
  CU_ASSERT(struct_ptr == NULL);
  size_t avail_after = h_avail(h);
  size_t used_after = h_used(h);
  CU_ASSERT(avail_before == avail_after);
  CU_ASSERT(used_before == used_after);
  h_delete(h);
}

/*============================================================================
 *                             h_alloc_data TESTING SUITE
 *===========================================================================*/

void
test_h_alloc_data_null_heap()
{
  void *data_ptr = h_alloc_data(NULL, sizeof(int));
  CU_ASSERT(data_ptr == NULL);
}

void
test_h_alloc_data_zero_bytes()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  void *data_ptr = h_alloc_data(h, 0);
  CU_ASSERT(data_ptr == NULL);
  h_delete(h);
}

void
test_h_alloc_data_one_byte()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  size_t avail_before = h_avail(h);
  size_t used_before = h_used(h);
  void *data_ptr = h_alloc_data(h, 1);
  CU_ASSERT(data_ptr != NULL);
  size_t avail_after = h_avail(h);
  size_t used_after = h_used(h);
  CU_ASSERT(avail_before > avail_after);
  CU_ASSERT(used_before < used_after);
  *(char *)data_ptr = 'a';
  h_delete(h);
}

void
test_h_alloc_data_100_bytes()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  size_t avail_before = h_avail(h);
  size_t used_before = h_used(h);
  void *data_ptr = h_alloc_data(h, 100);
  CU_ASSERT(data_ptr != NULL);
  size_t avail_after = h_avail(h);
  size_t used_after = h_used(h);
  CU_ASSERT(avail_before > avail_after);
  CU_ASSERT(used_before < used_after);
  h_delete(h);
}

void
test_h_alloc_data_too_big_for_page()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  size_t avail_before = h_avail(h);
  size_t used_before = h_used(h);
  void *data_ptr = h_alloc_data(h, 2049);
  CU_ASSERT(data_ptr == NULL);
  size_t avail_after = h_avail(h);
  size_t used_after = h_used(h);
  CU_ASSERT(avail_before == avail_after);
  CU_ASSERT(used_before == used_after);
  h_delete(h);
}



/*============================================================================
 *                             h_gc TESTING SUITE
 *===========================================================================*/

void
test_h_gc_null_heap_ptr()
{
  size_t cleaned = h_gc(NULL);
  CU_ASSERT(cleaned == 0);
}

void
test_h_gc_empty_heap()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  size_t cleaned = h_gc(h);
  CU_ASSERT(cleaned == 0);
  h_delete(h);
}

void
test_h_gc_no_garbage()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  void *data_ptr = h_alloc_data(h, sizeof(int));
  void **original_ptr = back_up_ptr(data_ptr);

  size_t avail_before = h_avail(h);
  size_t used_before = h_used(h);
  size_t cleaned = h_gc(h);
  size_t avail_after = h_avail(h);
  size_t used_after = h_used(h);
  CU_ASSERT(avail_before == avail_after);
  CU_ASSERT(used_before == used_after);
  CU_ASSERT(cleaned == 0);

  CU_ASSERT(data_ptr != *original_ptr);
  free(original_ptr);
  h_delete(h);
}

void
test_h_gc_only_garbage()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  void *data_ptr = h_alloc_data(h, sizeof(int));
  if(data_ptr){};//to silence gcc compiling whining....
  data_ptr = NULL;


  size_t avail_before = h_avail(h);
  size_t used_before = h_used(h);
  size_t cleaned = h_gc(h);
  size_t avail_after = h_avail(h);
  size_t used_after = h_used(h);
  CU_ASSERT(avail_before < avail_after);
  CU_ASSERT(used_before > used_after);
  CU_ASSERT(used_after == 0);

  CU_ASSERT(cleaned != 0);
  h_delete(h);
}

void
test_h_gc_mix()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  void *struct_ptr = h_alloc_struct(h, TEST_STRUCT_FORMAT_STR);
  void **original_ptr = back_up_ptr(struct_ptr);
  void *data_ptr = h_alloc_data(h, sizeof(int));
  if(data_ptr){};//to silence gcc compiling whining....
  data_ptr = NULL;
  size_t avail_before = h_avail(h);
  size_t used_before = h_used(h);
  size_t cleaned = h_gc(h);
  size_t avail_after = h_avail(h);
  size_t used_after = h_used(h);
  CU_ASSERT(avail_before < avail_after);
  CU_ASSERT(used_before > used_after);
  CU_ASSERT(used_after != 0);
  CU_ASSERT(cleaned != 0);
  CU_ASSERT(struct_ptr != *original_ptr);
  free(original_ptr);
  h_delete(h);
}

void
test_h_gc_ptr_inside_struct_no_garbage()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  test_struct_t *struct1_ptr = h_alloc_struct(h, TEST_STRUCT_FORMAT_STR);
  test_struct_t *struct2_ptr = h_alloc_struct(h, TEST_STRUCT_FORMAT_STR);
  *struct1_ptr = (test_struct_t){1, 'a', struct2_ptr, 2, 1.0};
  *struct2_ptr = (test_struct_t){1, 'a', NULL, 2, 1.0};

  void **original_ptr1 = back_up_ptr(struct1_ptr);
  void **original_ptr2 = back_up_ptr(struct2_ptr);
  
  size_t cleaned = h_gc(h);
  CU_ASSERT(cleaned == 0);

  CU_ASSERT(struct1_ptr != *original_ptr1);
  CU_ASSERT(struct2_ptr != *original_ptr2);
 
  free(original_ptr1);
  free(original_ptr2);
  h_delete(h);
}

void
test_h_gc_ptr_inside_struct_garbage()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  test_struct_t *struct1_ptr = h_alloc_struct(h, TEST_STRUCT_FORMAT_STR);
  test_struct_t *struct2_ptr = h_alloc_struct(h, TEST_STRUCT_FORMAT_STR);
  *struct1_ptr = (test_struct_t){1, 'a', struct2_ptr, 2, 1.0};
  *struct2_ptr = (test_struct_t){1, 'a', NULL, 2, 1.0};
  struct1_ptr = NULL;
  struct2_ptr = NULL;
  size_t cleaned = h_gc(h);
  CU_ASSERT(cleaned != 0);
  CU_ASSERT(h_used(h) == 0);
  h_delete(h);
}

void
test_h_gc_dbg_null_heap_ptr()
{
  size_t cleaned = h_gc_dbg(NULL, SAFE_STACK);
  CU_ASSERT(cleaned == 0);
}

void
test_h_gc_dbg_no_garbage()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  void *data_ptr = h_alloc_data(h, sizeof(int));
  void **original_ptr = back_up_ptr(data_ptr);

  size_t cleaned = h_gc_dbg(h, UNSAFE_STACK);
  CU_ASSERT(cleaned == 0);

  CU_ASSERT(data_ptr == *original_ptr);
  free(original_ptr);
  h_delete(h);
}

void
test_h_gc_dbg_only_garbage()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  void *data_ptr = h_alloc_data(h, sizeof(int));
  if(data_ptr){};//to silence gcc compiling whining....
  data_ptr = NULL;

  size_t avail_before = h_avail(h);
  size_t used_before = h_used(h);
  size_t cleaned = h_gc_dbg(h, UNSAFE_STACK);
  size_t avail_after = h_avail(h);
  size_t used_after = h_used(h);
  CU_ASSERT(avail_before < avail_after);
  CU_ASSERT(used_before > used_after);
  CU_ASSERT(used_after == 0);

  CU_ASSERT(cleaned != 0);
  h_delete(h);
}

void
test_h_gc_dbg_ptr_inside_struct_no_garbage()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  test_struct_t *struct1_ptr = h_alloc_struct(h, TEST_STRUCT_FORMAT_STR);
  test_struct_t *struct2_ptr = h_alloc_struct(h, TEST_STRUCT_FORMAT_STR);
  *struct1_ptr = (test_struct_t){1, 'a', struct2_ptr, 2, 1.0};
  *struct2_ptr = (test_struct_t){1, 'a', NULL, 2, 1.0};
  struct2_ptr = NULL;

  void **original_ptr1 = back_up_ptr(struct1_ptr);
  
  size_t cleaned = h_gc_dbg(h, UNSAFE_STACK);
  CU_ASSERT(cleaned == 0);

  CU_ASSERT(struct1_ptr == *original_ptr1);
  free(original_ptr1);
  h_delete(h);
}

void
test_h_gc_dbg_ptr_inside_struct_internal_garbage()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  test_struct_t *struct1_ptr = h_alloc_struct(h, TEST_STRUCT_FORMAT_STR);
  test_struct_t *struct2_ptr = h_alloc_struct(h, TEST_STRUCT_FORMAT_STR);
  *struct1_ptr = (test_struct_t){1, 'a', struct2_ptr, 2, 1.0};
  *struct2_ptr = (test_struct_t){1, 'a', NULL, 2, 1.0};

  struct1_ptr->ptr = NULL;
  struct2_ptr = NULL;

  void **original_ptr1 = back_up_ptr(struct1_ptr);
  
  size_t cleaned = h_gc_dbg(h, UNSAFE_STACK);
  CU_ASSERT(cleaned == 0); // No GC if page is UNSAFE

  CU_ASSERT(struct1_ptr == *original_ptr1);
  free(original_ptr1);
  h_delete(h);
}

/*============================================================================
 *                             h_avail TESTING SUITE
 *===========================================================================*/


void
test_h_avail_null_heap()
{
  CU_ASSERT(h_avail(NULL) == 0);
}

void
test_h_avail_zero_alloced()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  h_alloc_data(h, 0);
  CU_ASSERT(h_avail(h) == SMALLEST_HEAP_SIZE);
  h_delete(h);
}


void
test_h_avail_too_big_allocated()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  h_alloc_data(h, 2049);
  CU_ASSERT(h_avail(h) == SMALLEST_HEAP_SIZE);
  h_delete(h);
}

void
test_h_avail_invalid_str()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  h_alloc_struct(h, "asd");
  CU_ASSERT(h_avail(h) == SMALLEST_HEAP_SIZE);
  h_delete(h);
}

void
test_h_avail_emty_str()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  h_alloc_struct(h, "");
  CU_ASSERT(h_avail(h) == SMALLEST_HEAP_SIZE);
  h_delete(h);
}

void
test_h_avail_valid_struct()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  h_alloc_struct(h, "c");
  CU_ASSERT(h_avail(h) == SMALLEST_HEAP_SIZE-16);
  h_delete(h);
}


void
test_h_avail_valid_data()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  h_alloc_data(h, 8);
  CU_ASSERT(h_avail(h) == SMALLEST_HEAP_SIZE-16);
  h_delete(h);
}

void
test_h_avail_valid_one()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  h_alloc_data(h, 1);
  CU_ASSERT(h_avail(h) == SMALLEST_HEAP_SIZE-16);
  h_delete(h);
}


void
test_h_avail_valid_bigger()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  h_alloc_data(h, 100);
  CU_ASSERT(h_avail(h) == SMALLEST_HEAP_SIZE-112);
  h_delete(h);
}



/*============================================================================
 *                             h_used TESTING SUITE
 *===========================================================================*/


void
test_h_used_null_heap()
{
  CU_ASSERT(h_avail(NULL) == 0);
}

void
test_h_used_zero_alloced()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  h_alloc_data(h, 0);
  CU_ASSERT(h_used(h) == 0);
  h_delete(h);
}


void
test_h_used_too_big_allocated()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  h_alloc_data(h, 2049);
  CU_ASSERT(h_used(h) == 0);
  h_delete(h);
}

void
test_h_used_invalid_str()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  h_alloc_struct(h, "asd");
  CU_ASSERT(h_used(h) == 0);
  h_delete(h);
}

void
test_h_used_emty_str()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  h_alloc_struct(h, "");
  CU_ASSERT(h_used(h) == 0);
  h_delete(h);
}

void
test_h_used_valid_struct()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  h_alloc_struct(h, "c");
  CU_ASSERT(h_used(h) == 16);
  h_delete(h);
}


void
test_h_used_valid_data()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  h_alloc_data(h, 8);
  CU_ASSERT(h_used(h) == 16);
  h_delete(h);
}

void
test_h_used_valid_one()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  h_alloc_data(h, 1);
  CU_ASSERT(h_used(h) == 16);
  h_delete(h);
}


void
test_h_used_valid_bigger()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  h_alloc_data(h, 100);
  CU_ASSERT(h_used(h) == 112);
  h_delete(h);
}

/*============================================================================
 *                             h_size TESTING SUITE
 *===========================================================================*/

void
test_h_size_smallest()
{
  heap_t *h = h_init(SMALLEST_HEAP_SIZE, SAFE_STACK, 0.5);
  CU_ASSERT(h_size(h) == SMALLEST_HEAP_SIZE);
  h_alloc_data(h, 100);
  CU_ASSERT(h_size(h) == h_avail(h) + h_used(h));
  h_delete(h);
}


/*============================================================================
 *                             Old school TESTING SUITE
 *===========================================================================*/

void
test_h_alloc ()
{
  int test_size = 6144;
  heap_t *test_h_alloc_heap = h_init(test_size, true, 1);

  void * ptr1 = h_alloc_struct(test_h_alloc_heap, "i");
  write_int_to_heap(ptr1, 6);

  void * ptr2 = h_alloc_struct(test_h_alloc_heap, "i");
  write_int_to_heap(ptr2, 9);

  void * ptr4 = h_alloc_data(test_h_alloc_heap, 2040);

  void * ptr5 = h_alloc_struct(test_h_alloc_heap, "i*i");
  struct test_struct ts = ( (struct test_struct) {1, 'c', &ptr5, 3, 2}); 
  *(struct test_struct *)ptr5 = ts;
  

  void * ptr3 = h_alloc_struct(test_h_alloc_heap, "*");
  void * test_pointer = malloc(10);
  write_pointer_to_heap(&ptr3, test_pointer);

  CU_ASSERT(*(int *)ptr1 == 6);
  CU_ASSERT(*(int *)ptr2 == 9);
  CU_ASSERT_PTR_EQUAL(test_pointer, ptr3);
  CU_ASSERT(ptr4 != NULL);
  CU_ASSERT(ptr5 != NULL);

  h_delete(test_h_alloc_heap);
  free(test_pointer);
}

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

  get_ptrs_from_stack(h, stack_top, collection, number_of_ptrs);
  CU_ASSERT(*(int *) *collection[1] == 6); 
  CU_ASSERT(*(int *) *collection[0] == 9); 
  h_delete(h); 

}

void
test_get_ptr_page()
{
  int test_size = 6144;
  heap_t *test_h_alloc_heap = h_init(test_size, true, 1); 

  void * ptr1 = h_alloc_struct(test_h_alloc_heap, "i");
  write_int_to_heap(ptr1, 6);

  CU_ASSERT(get_ptr_page(test_h_alloc_heap, ptr1) != -1);
  h_delete(test_h_alloc_heap);
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
  
  h_delete(test_h_alloc_heap_passive);
}




/*============================================================================
 *                             Main Testing Function
 *===========================================================================*/
int
main (void)
{
  CU_pSuite suite1 = NULL;
  CU_pSuite suite_h_init = NULL;
  CU_pSuite suite_h_delete = NULL;
  CU_pSuite suite_h_alloc_struct = NULL;
  CU_pSuite suite_h_alloc_data = NULL;
  CU_pSuite suite_h_gc = NULL;
  CU_pSuite suite_h_avail = NULL;
  CU_pSuite suite_h_used = NULL;
  CU_pSuite suite_h_size = NULL;

  if (CU_initialize_registry() != CUE_SUCCESS)
    {
      return CU_get_error();
    }


  // ********************* h_init SUITE ******************  //
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

  // ********************* h_delete SUITE ******************  //
  suite_h_delete = CU_add_suite("Tests function h_delete() and h_delete_dbg()",
                                NULL, NULL);
  if (suite_h_delete == NULL) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if ( (NULL == CU_add_test(suite_h_delete
                            , "null heap ptr"
                            , test_h_delete_null_ptr) ) ||
       (NULL == CU_add_test(suite_h_delete
                               , "dbg null heap ptr"
                               , test_h_delete_dbg_null_ptr) ) ||
       (NULL == CU_add_test(suite_h_delete
                               , "null dbg value"
                               , test_h_delete_dbg_null_value) )
       || (NULL == CU_add_test(suite_h_delete
                               , "other dbg value"
                               , test_h_delete_dbg_other_value) )
       || (NULL == CU_add_test(suite_h_delete
                               , "multiple ptrs to same data"
                               , test_h_delete_dbg_multiple_ptrs_to_same_data) )
       || (NULL == CU_add_test(suite_h_delete
                               , "multiple ptrs different data"
                               , test_h_delete_dbg_multiple_ptrs) )
       || (NULL == CU_add_test(suite_h_delete
                               , "ptr into unused area in heap"
                               , test_h_delete_dbg_ptr_into_unused_heap_area) )
       || (NULL == CU_add_test(suite_h_delete
                               , "struct allocated"
                               , test_h_delete_dbg_struct) )
    )
    {
      CU_cleanup_registry();
      return CU_get_error();
    }


  // ********************* h_alloc_struct SUITE ******************  //
  suite_h_alloc_struct = CU_add_suite("Tests function h_alloc_struct()",
                                NULL, NULL);
  if (suite_h_alloc_struct == NULL) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if ( (NULL == CU_add_test(suite_h_alloc_struct
                               , "null heap"
                               , test_h_alloc_struct_null_heap) ) ||
       (NULL == CU_add_test(suite_h_alloc_struct
                               , "null format string"
                               , test_h_alloc_struct_null_str) ) ||
       (NULL == CU_add_test(suite_h_alloc_struct
                               , "empty format string"
                               , test_h_alloc_struct_empty_str) ) ||
        (NULL == CU_add_test(suite_h_alloc_struct
                               , "invalid format string"
                               , test_h_alloc_struct_invalid_str) ) ||
        (NULL == CU_add_test(suite_h_alloc_struct
                               , "valid str"
                               , test_h_alloc_struct_valid_str) ) ||
        (NULL == CU_add_test(suite_h_alloc_struct
                               , "stuct too big for page"
                               , test_h_alloc_struct_too_big_for_page) )
    )
    {
      CU_cleanup_registry();
      return CU_get_error();
    }


  // ********************* h_alloc_data SUITE ******************  //
  suite_h_alloc_data = CU_add_suite("Tests function h_alloc_data()",
                                NULL, NULL);
  if (suite_h_alloc_data == NULL) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if ( (NULL == CU_add_test(suite_h_alloc_data
                               , "null heap"
                               , test_h_alloc_data_null_heap) ) ||
       (NULL == CU_add_test(suite_h_alloc_data
                               , "zero bytes"
                               , test_h_alloc_data_zero_bytes) ) ||
       (NULL == CU_add_test(suite_h_alloc_data
                               , "one byte"
                               , test_h_alloc_data_one_byte) ) ||
        (NULL == CU_add_test(suite_h_alloc_data
                               , "100 bytes"
                               , test_h_alloc_data_100_bytes) ) ||
        (NULL == CU_add_test(suite_h_alloc_data
                               , "too big for a page"
                               , test_h_alloc_data_too_big_for_page) )
    )
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  
  // ********************* h_gc SUITE ******************  //
  suite_h_gc = CU_add_suite("Tests function h_gc() and h_gc_dbg()",
                                NULL, NULL);
  if (suite_h_gc == NULL) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if ( (NULL == CU_add_test(suite_h_gc
                               , "null heap ptr"
                               , test_h_gc_null_heap_ptr) ) ||
       (NULL == CU_add_test(suite_h_gc
                               , "empty heap"
                               , test_h_gc_empty_heap) ) ||
       (NULL == CU_add_test(suite_h_gc
                               , "no garbage in heap"
                               , test_h_gc_no_garbage) ) ||
        (NULL == CU_add_test(suite_h_gc
                               , "only garbage in heap"
                               , test_h_gc_only_garbage) ) ||
        (NULL == CU_add_test(suite_h_gc
                               , "mix of garbage and living in heap"
                               , test_h_gc_mix) ) ||
        (NULL == CU_add_test(suite_h_gc
                               , "ptr inside struct no garbage"
                               , test_h_gc_ptr_inside_struct_no_garbage) ) ||
        (NULL == CU_add_test(suite_h_gc
                               , "ptr inside struct garbage"
                               , test_h_gc_ptr_inside_struct_garbage) ) ||
        (NULL == CU_add_test(suite_h_gc
                               , "dbg: no garbage"
                               , test_h_gc_dbg_no_garbage) ) ||
        (NULL == CU_add_test(suite_h_gc
                               , "dbg: only garbage in heap"
                               , test_h_gc_dbg_only_garbage) ) ||
       (NULL == CU_add_test(suite_h_gc
                               , "dgb: null heap ptr"
                               , test_h_gc_dbg_null_heap_ptr) )
    )
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  // ********************* h_avail SUITE ******************  //
  suite_h_avail = CU_add_suite("Tests function h_avail()",
                                NULL, NULL);
  if (suite_h_alloc_data == NULL) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if ( (NULL == CU_add_test(suite_h_avail
                            , "null heap"
                            , test_h_avail_null_heap) ) ||
       (NULL == CU_add_test(suite_h_avail
                            , "zero allocated"
                            , test_h_avail_zero_alloced) ) || 
       (NULL == CU_add_test(suite_h_avail
                            , "too big allocated"
                            , test_h_avail_too_big_allocated) ) || 
       (NULL == CU_add_test(suite_h_avail
                            , "invalid string"
                            , test_h_avail_invalid_str) ) || 
       (NULL == CU_add_test(suite_h_avail
                            , "empty string"
                            , test_h_avail_emty_str) ) ||
       (NULL == CU_add_test(suite_h_avail
                            , "valid struct"
                            , test_h_avail_valid_struct) ) ||
       (NULL == CU_add_test(suite_h_avail
                            , "valid data"
                            , test_h_avail_valid_data) ) ||
       (NULL == CU_add_test(suite_h_avail
                            , "one byte"
                            , test_h_avail_valid_one) ) ||
       (NULL == CU_add_test(suite_h_avail
                            , "100 byte"
                            , test_h_avail_valid_bigger) ) 

    )
    {
      CU_cleanup_registry();
      return CU_get_error();
    }



  // ********************* h_used SUITE ******************  //
  suite_h_used = CU_add_suite("Tests function h_used()",
                                NULL, NULL);
  if (suite_h_alloc_data == NULL) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if ( (NULL == CU_add_test(suite_h_used
                            , "null heap"
                            , test_h_used_null_heap) ) ||
       (NULL == CU_add_test(suite_h_used
                            , "zero allocated"
                            , test_h_used_zero_alloced) ) || 
       (NULL == CU_add_test(suite_h_used
                            , "too big allocated"
                            , test_h_used_too_big_allocated) ) || 
       (NULL == CU_add_test(suite_h_used
                            , "invalid string"
                            , test_h_used_invalid_str) ) || 
       (NULL == CU_add_test(suite_h_used
                            , "empty string"
                            , test_h_used_emty_str) ) ||
       (NULL == CU_add_test(suite_h_used
                            , "valid struct"
                            , test_h_used_valid_struct) ) ||
       (NULL == CU_add_test(suite_h_used
                            , "valid data"
                            , test_h_used_valid_data) ) ||
       (NULL == CU_add_test(suite_h_used
                            , "one byte"
                            , test_h_used_valid_one) ) ||
       (NULL == CU_add_test(suite_h_used
                            , "1000 byte"
                            , test_h_used_valid_bigger) ) 

    )
    {
      CU_cleanup_registry();
      return CU_get_error();
    }



  // ********************* h_size SUITE ******************  //
  suite_h_size = CU_add_suite("Tests function h_size()",
                                NULL, NULL);
  if (suite_h_alloc_data == NULL) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if ( (NULL == CU_add_test(suite_h_size
                               , "size of the heap"
                               , test_h_size_smallest) ) 
    )
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  
  // ********************* Legacy  SUITE ******************  //
  suite1 = CU_add_suite("Legacy Test", NULL, NULL);

  if (
      (CU_add_test(suite1, "test_h_alloc_struct/data()", test_h_alloc) == NULL) ||
      (CU_add_test(suite1, "test_ptrs_from_stack", test_ptrs_from_stack) == NULL) ||
      (CU_add_test(suite1, "test_number_of_ptrs_in_stack", test_get_number_of_ptrs_in_stack) == NULL) ||
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
