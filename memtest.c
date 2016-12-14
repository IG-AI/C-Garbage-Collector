#include <stdio.h>
#include <stdlib.h>
#include "gc.h"

int main (){
  int test_size = 6144;
  //int page_size = 2048;
  heap_t *h = h_init(test_size, true, 1);

  h_delete(h);
  return 0;
}

