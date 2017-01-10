#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "CUnit/Basic.h"
#include "stack_search.h"

extern char **environ;

void * get_stack_top(){
  int dummy = 0;
  void *ptr_dummy = &dummy;
  return ptr_dummy;
}

void test_stack_find_ptr()
{
  /**
   *  Test a middle case of stack_find_next_ptr()
   *  Will look for *target on the stack
   */

  //allocates a value on the heap
  int *target = calloc(sizeof(int), 1);

  //Get the top of the stack using __builtin_frame_address(0);
  void *stack_top = get_stack_top();

  /**
   *  Creates a pointer containing the same value as *environ
   *  we need to create this in a new pointer since this value
   *  is updated by stack_find_next_ptr()
   */
  void *stack_bottom = (void *)*environ;

  //set a value to *target
  *target = 111;

  /**
   *  Using the heap address of target, we will create a range of valid
   *  heap addresses to look for.
   */
  void *heap_end = target + 40;
  void *heap_start = target - 40;

  bool finished = false;
  bool found_addr = false;

  while(!finished)
    {
      void **pointer = stack_find_next_ptr(&stack_bottom, stack_top, heap_start, heap_end);
       if(pointer == NULL)
        {
          finished = true;
        }
      else if(*(unsigned long *)pointer == (unsigned long)target)
        {
          found_addr = true;        
         }
    }
  CU_ASSERT_TRUE(found_addr);
  free(target);
}



void test_stack_edges()
{
  /**
   * Tests the edge cases for stack_find_next_ptr()
   */
  
  //allocates some memory on heap
  int times = 12;
  int *mem_start = calloc(times, sizeof(int));

  /**
   * Creates some pointers to the heap in the above allocated space.
   * mem_start will be below the searched heap range.
   * mem_before will be one byte before it.
   * mem_lower_inside will be the first byte of the heap range.
   * mem_one_above_inside will be one byte inside it.
   * mem_middle will be in middle.
   * mem_one_below_end will be one byte less than the end.
   * mem_upper_inside will be the last address(byte) of the heap range.
   * mem_after will be one byte over the heap_range
   */
  int *mem_before = (int *)((unsigned long)mem_start+3);
  int *mem_lower_inside = mem_start+1;
  int *mem_one_above_inside = (int *)((unsigned long)mem_lower_inside+1);
  int *mem_middle = mem_start+5;
  int *mem_upper_end = mem_start + 9;
  int *mem_one_below_end = (int *)((unsigned long)mem_upper_end-1);
  int *mem_after = (int *)((unsigned long)mem_upper_end+1);

  //put some values to some of the pointers, nor really needed.. 
  //..and sparc will cry
  /**mem_start = 0;
  *mem_before = 0;
  *mem_lower_inside = 1;
  *mem_one_above_inside = 0;
  *mem_middle = 5;
  *mem_one_below_end = 2;
  *mem_upper_end = 9;
  *mem_after = 10;
  */
  /*
  printf("mem_start:     %p\n", mem_start);
  printf("mem_before:    %p\n", mem_before);
  printf("mem_lower_in:  %p\n", mem_lower_inside);
  printf("mem_one_ab_in: %p\n", mem_one_above_inside);
  printf("mem_middle:    %p\n", mem_middle);
  printf("mem_one_be_end:%p\n", mem_one_below_end);
  printf("mem_upper_end: %p\n", mem_upper_end);
  printf("mem_after:     %p\n", mem_after);*/
  

  //Get the top of the stack using __builtin_frame_address(0);
  void *stack_top = get_stack_top();

  /**
   *  Creates a pointer containing the same value as *environ
   *  we need to create this in a new pointer since this value
   *  is updated by stack_find_next_ptr()
   */
  void *stack_bottom = (void *)*environ;

  
  bool finished = false;

  bool found_start = false;
  bool found_before = false;
  bool found_lower_inside = false;
  bool found_one_above_inside = false;
  bool found_middle = false;
  bool found_one_below_end = false;
  bool found_upper_end = false;
  bool found_after = false;
 
  /**
   *  In the loop each pointer returned from stack_find_next_ptr()
   *  is matched to one of the pointers we created, and if found
   *  the corresponding bool will be set to true.
   *  In theory we can get more matches than on the pointers we created
   *  but they will not affect the end result of the test.
   */

  while(!finished)
    {
      void **pointer = stack_find_next_ptr(&stack_bottom, stack_top, mem_lower_inside, mem_upper_end);        

      if(pointer == NULL)
        {
          finished = true;
        }
      else if(*(unsigned long *)pointer == (unsigned long) mem_start)
        {          
          found_start = true;  
        }
      else if(*(unsigned long *)pointer == (unsigned long) mem_before)
        {          
          found_before = true;  
        }
      else if(*(unsigned long *)pointer == (unsigned long) mem_lower_inside)
        {          
          found_lower_inside = true;  
        }
      else if(*(unsigned long *)pointer == (unsigned long) mem_one_above_inside)
        {          
          found_one_above_inside = true;
        }
      else if (*(unsigned long *)pointer == (unsigned long) mem_middle) 
        {      
          found_middle = true;  
        }
      else if(*(unsigned long *)pointer == (unsigned long) mem_one_below_end)
        {          
          found_one_below_end = true;
        }
      else if (*(unsigned long *)pointer == (unsigned long) mem_upper_end)
        {          
          found_upper_end = true;        
        }
      else if(*(unsigned long *)pointer == (unsigned long) mem_after)
        {          
          found_after = true;
        }
    }

  CU_ASSERT_FALSE(found_start);
  CU_ASSERT_FALSE(found_before);
  CU_ASSERT_TRUE(found_lower_inside);
  CU_ASSERT_TRUE(found_one_above_inside);
  CU_ASSERT_TRUE(found_middle);
  CU_ASSERT_TRUE(found_one_below_end);
  CU_ASSERT_TRUE(found_upper_end);
  CU_ASSERT_FALSE(found_after);

}



int main(int argc, char *argv[]){

  if(argc && argv){}
  CU_initialize_registry();

  //Set up suites and tests for stack functions
  CU_pSuite stack_test = CU_add_suite("Test stack search", NULL, NULL);
  CU_add_test(stack_test, "Test_stack_find_ptr", test_stack_find_ptr);
  CU_add_test(stack_test, "Test_stack_edges", test_stack_edges);

  //Actually run tests
  CU_basic_run_tests();

  //Tear down
  CU_cleanup_registry();
  return 0;
}
