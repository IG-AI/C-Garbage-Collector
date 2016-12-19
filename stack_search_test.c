#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "CUnit/Basic.h"
#include "stack_search.h"

extern char **environ;

void * get_end_of_stack(){
  return __builtin_frame_address(0);
}

void test_stack_find_ptr()
{
  int *target = calloc(sizeof(int), 1);
  void *stack_top = get_end_of_stack();
  
  void *stack_bottom = (void *)*environ;
  *target = 111;
  void *heap_end = target + 40;
  void *heap_start = target - 40;
  bool finished = false;
  bool found_addr = false;
  int expected_value = 0;
  while(!finished)
    {
      void *pointer = stack_find_next_ptr(&stack_bottom, stack_top, heap_start, heap_end);
      
      if(pointer == NULL)
        {
          finished = true;
        }
      else if(*(unsigned long *)pointer == (unsigned long)target)
        {
          expected_value = *(int*)*(void **)pointer;           
          found_addr = true;        
        }      
    }
  CU_ASSERT_TRUE(found_addr);
  CU_ASSERT_EQUAL(expected_value, *target);
  free(target);
}



void test_stack_edges()
{
  int times = 12;
  int *mem_start = calloc(times, sizeof(int));
  int *mem_lower_inside = mem_start+1;
  int *mem_middle = mem_start+5;
  int *mem_upper_inside = mem_start + 9;
  int *mem_after = mem_start + 10;
  *mem_start = 0;
  *mem_lower_inside = 1;
  *mem_middle = 5;
  *mem_upper_inside = 9;
  *mem_after = 10;

  void *stack_top = get_end_of_stack();
  void *stack_bottom = (void *)*environ;
  bool finished = false;
  bool found_lower_inside = false;
  bool found_middle = false;
  bool found_upper_inside = false;
 
  while(!finished)
    {
      
      void *pointer = stack_find_next_ptr(&stack_bottom, stack_top, mem_lower_inside, mem_upper_inside);
           
      if(pointer == NULL)
        {
          finished = true;
        }
      else if(*(unsigned long *)pointer == (unsigned long) mem_lower_inside)
        {          
          found_lower_inside = true;  
        }
      else if (*(unsigned long *)pointer == (unsigned long) mem_middle) 
        {      
          found_middle = true;  
        }
      else if (*(unsigned long *)pointer == (unsigned long) mem_upper_inside)
        {          
          found_upper_inside = true;        
        }
    }

  CU_ASSERT_TRUE(found_lower_inside);
  CU_ASSERT_TRUE(found_middle);
  CU_ASSERT_TRUE(found_upper_inside);

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
