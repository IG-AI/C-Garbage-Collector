#include <string.h>
#include <stdlib.h>
#include "CUnit/Basic.h"

extern char **environ;

//does nothing.. useful as delete_func for items stored on the stack..
void do_nothing(){
}

void test_2(){
  void *top = __builtin_frame_address(1);
  printf("Top3:     %p\n", top);

}

///////TESTS FOR LIST FUNCTIONS///////
void test_list_new(){


 

  int *i = malloc(sizeof(int));

  int b = 12;
  int c = 23;
  int d = 45;
  int e = 45;
  int f = 45;
  int g = 45;
  int h = 45;
  int j = 45;
 
  int *k = alloca(sizeof(int));
  int *l = alloca(sizeof(int));
  int *m = alloca(sizeof(int));
  int *n = alloca(sizeof(int));
  int *o = alloca(sizeof(int));
  int *p = alloca(sizeof(int));
  int *q = alloca(sizeof(int));
  int *r = alloca(sizeof(int));
  int *s = alloca(sizeof(int));

  void *top = __builtin_frame_address(0);
  
 
  void *bottom = environ;
   

  int **heap_start = &i - 1;
  int **heap_end = &i + 1;
  //  unsigned long q = (unsigned long)&i;
  void *top2 = __builtin_frame_address(0);
  printf("Bottom:   %p\n", bottom);
  printf("Top:      %p\n", top);
  printf("Malloced: %p\n", &i);
  printf("Int b:    %p\n", &b);
  printf("Int c:    %p\n", &c);
  printf("Int d:    %p\n", &d);
  printf("Int e:    %p\n", &e);
  printf("Int f:    %p\n", &f);
  printf("Int g:    %p\n", &g);
  printf("Int h:    %p\n", &h);
  printf("Int j:    %p\n", &j);

  puts("");
  printf("Int pk:   %p\n", &k);
  printf("Int vk:   %p\n", k);
  puts("");
  printf("Int pl:   %p\n", &l);
  printf("Int vl:   %p\n", l);
  puts("");
  printf("Int pm:   %p\n", &m);
  printf("Int vm:   %p\n", m);
  puts("");
  printf("Int pn:   %p\n", &n);
  printf("Int vn:   %p\n", n);
  puts("");
  printf("Int po:   %p\n", &o);
  printf("Int vo:   %p\n", o);
  puts("");
  printf("Int pp:   %p\n", &p);
  printf("Int vp:   %p\n", p);
  puts("");
  printf("Int pq:   %p\n", &q);
  printf("Int vq:   %p\n", q);
  puts("");
  printf("Int pr:   %p\n", &r);
  printf("Int vr:   %p\n", r);
  puts("");
  printf("Int ps:   %p\n", &s);
  printf("Int vs:   %p\n", s);

  // printf("Int q:    %lu\n", q);
  printf("M + 4:    %p\n", heap_start);
  printf("M - 4:    %p\n", heap_end);
  printf("Top2:     %p\n", top2);
  test_2();
  /* CU_ASSERT_PTR_NULL(list_first(list)); */
  /* CU_ASSERT_PTR_NULL(list_last(list)); */
  
  /* list_free(list, (delete_list_item)do_nothing); */

   free(i);
}



int main(int argc, char *argv[]){
  if(argc && argv){}
  CU_initialize_registry();

  //Set up suites and tests for list functions
  CU_pSuite list_new = CU_add_suite("Test creation of list", NULL, NULL);
  CU_add_test(list_new, "Test list_new", test_list_new);

 

  //Actually run tests
  CU_basic_run_tests();

  //Tear down
  CU_cleanup_registry();
  return 0;
}
