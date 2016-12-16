#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "CUnit/Basic.h"
#include "stack_search.h"

extern char **environ;

//does nothing.. useful as delete_func for items stored on the stack..
void do_nothing(){
}


void * get_end_of_stack(){
  return __builtin_frame_address(0);
}

void test_stack_find_ptr()
{
  int *target = calloc(sizeof(int), 1);
  void *stack_end = get_end_of_stack();
  void **stack_start = (void **)environ;
  *target = 111;
  void *heap_end = target + 40;
  void *heap_start = target - 40;
  bool finished = false;
  bool found_addr = false;
  int expected_value = 0;
  int count = -1;
  printf("target:     %p\n", &target);
  while(!finished)
    {
      void *pointer = stack_find_next_ptr(stack_start, stack_end, heap_start, heap_end);
            
      ++count;
      
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
  printf("-----------------------------------------------------\n");
}



void test_stack()
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

  void *stack_end = get_end_of_stack();
  void **stack_start = (void **)environ;
  bool finished = false;
  bool found_lower_inside = false;
  bool found_middle = false;
  bool found_upper_inside = false;
  int count = -1;
  // printf("stack_top:     %p\n", stack_top);
  printf("mem_start:     %p\n", &mem_start);
  printf("mem_lower_in:  %p\n", &mem_lower_inside);
  printf("mem_middle:    %p\n", &mem_middle);
  printf("mem_upper_int: %p\n", &mem_upper_inside);
  printf("mem_after:     %p\n", &mem_after);
  while(!finished)
    {
      void *pointer = stack_find_next_ptr(stack_start, stack_end, mem_lower_inside-40, mem_upper_inside+40);
            
      ++count;
      
      if(pointer == NULL)
        {
          finished = true;
        }
      else if(*(unsigned long *)pointer == (unsigned long) mem_lower_inside)
        {
          printf("li %d\n", *(int*)*(void **)pointer);
          found_lower_inside = true;  
        }
      else if (*(unsigned long *)pointer == (unsigned long) mem_middle) 
        {
          printf("m %d\n", *(int*)*(void **)pointer);
          found_middle = true;  
        }
      else if (*(unsigned long *)pointer == (unsigned long) mem_upper_inside)
        {
          printf("ui %d\n", *(int*)*(void **)pointer);
          found_upper_inside = true;        
        }
      else
        {
          puts("nothing");
        }
    }

  printf("st:    %p\n", mem_start);
  printf("st:    %p\n", mem_lower_inside);
  printf("st:    %p\n", mem_middle);
  printf("st:    %p\n", mem_upper_inside);
  printf("st:    %p\n", mem_after);
  printf("st:    %d\n", *mem_start);
  printf("st:    %d\n", *mem_lower_inside);
  printf("st:    %d\n", *mem_middle);
  printf("st:    %d\n", *mem_upper_inside);
  printf("st:    %d\n", *mem_after);

  CU_ASSERT_TRUE(found_lower_inside);
  CU_ASSERT_TRUE(found_middle);
  CU_ASSERT_TRUE(found_upper_inside);

  printf("count: %d", count);

}



int main(int argc, char *argv[]){

  if(argc && argv){}
  CU_initialize_registry();

  //Set up suites and tests for stack functions
  CU_pSuite stack_test = CU_add_suite("Test stack search", NULL, NULL);
  CU_add_test(stack_test, "Test_stack_find_ptr", test_stack_find_ptr);
  CU_add_test(stack_test, "Test_stack_large", test_stack);

  //Actually run tests
  CU_basic_run_tests();

  //Tear down
  CU_cleanup_registry();
  return 0;
}
