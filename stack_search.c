#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


void **stack_find_next_ptr(void **stack_bottom, void *stack_top, void *heap_start, void *heap_end)
{
  /*
   * Bitshifts the bottom value to align it to 8-byte addresses
   * Required for Sparc platforms which have addresses aligned
   * to 8-byte addresses. From our understanding this is also
   * true for the other platforms too.
   */
  *stack_bottom = (void *)((unsigned long)*stack_bottom >> 3UL);
  *stack_bottom = (void *)((unsigned long)*stack_bottom << 3UL);
  while(*stack_bottom > stack_top)
    {
      /*
       * Moves the pointer, sizeof(void*) will move it 8 bytes.
       * If future tests would show non-alignment in some platforms
       * move it instead sizeof(char).
       */
      *stack_bottom = (void *)(((unsigned long)*stack_bottom) - sizeof(void *));
      void *result = *stack_bottom;
      if( *(unsigned long *)result <= (unsigned long)heap_end 
          && *(unsigned long *)result >= (unsigned long)heap_start)
        {
          return stack_bottom;
        }
    }
  return NULL;
}


