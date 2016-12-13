#include <stdio.h>
#include <stdlib.h>
#include "gc_h_test.h"

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

void 
test_h_init ()
{
  CU_ASSERT(h_init(16, true, 1) != NULL); 
}

int main ()
{
  CU_pSuite suite1 = NULL;

 
}
