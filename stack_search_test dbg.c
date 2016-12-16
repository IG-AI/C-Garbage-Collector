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
void test_stack()
{
  int *mal = calloc(sizeof(int), 1);
  void *stack_end = get_end_of_stack();
  void **stack_start = (void **)environ;
  *mal = 111;
  void *heap_end = mal + 40;
  void *heap_start = mal - 40;
  printf("mal: %p\n", mal);
  printf("&mal:%p\n", &mal);
  printf("h end: %p\n", heap_end);
  printf("h start: %p\n", heap_start);

  printf("s end: %p\n", stack_end);
  printf("s start: %p\n", stack_start);
  bool finished = false;
  bool found_addr = false;
  int expected_value = 0;
  int count = -1;
  while(!finished)
    {
      void *re = stack_find_next_ptr(stack_start, stack_end, heap_start, heap_end);
      
      printf("re:  %p\n", re);
      ++count;
      
      if(re == NULL)
        {
          finished = true;
        }
      else if(*(unsigned long *)re == (unsigned long)mal)
        {
          expected_value = *(int*)*(void **)re; 
          //printf("rei: %d\n",*(int*)*(void **)re);
          found_addr = true;
          printf("reik:%p\n", *(void**)re);
          printf("rei: %d\n",*(int*)*(void **)re);
        }
      else
        {
          printf("rek: %p\n", *(void**)re);
        }
    }
  printf("Count: %d\n", count);
  CU_ASSERT_TRUE(found_addr);
  CU_ASSERT_EQUAL(expected_value, *mal);
  free(mal);
}





int main(int argc, char *argv[]){

  if(argc && argv){}
  CU_initialize_registry();

  //Set up suites and tests for stack functions
  CU_pSuite stack_test = CU_add_suite("Test stack search", NULL, NULL);
  CU_add_test(stack_test, "Test stack", test_stack);

  //Actually run tests
  CU_basic_run_tests();

  //Tear down
  CU_cleanup_registry();
  return 0;
}
