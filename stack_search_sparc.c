#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


void *stack_find_next_ptr(void **stack_top, void *stack_bottom, void *heap_start, void *heap_end)
{
  *stack_top = (void *)((unsigned long)*stack_top >> 3UL);
  *stack_top = (void *)((unsigned long)*stack_top << 3UL);
  printf("stack_top:     %p\n", stack_top);
  printf("stack_bottom:  %p\n", stack_bottom);
  printf("heap_start:    %p\n", heap_start);
  printf("heap_end:      %p\n", heap_end);
  while(*stack_top > stack_bottom)
    {
      void *result = *stack_top;
      if( *(unsigned long *)result <= (unsigned long)heap_end-sizeof(void *) 
          && *(unsigned long *)result >= (unsigned long)heap_start)
        {
          *stack_top = (void *)(((unsigned long)*stack_top) - sizeof(char));
          return result;
        }
      *stack_top = (void *)(((unsigned long)*stack_top) - sizeof(char));
    }
  return NULL;
}


