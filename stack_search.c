#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


#include <setjmp.h>

#define Dump_registers() \
  jmp_buf env; \
  if (setjmp(env)) abort(); \



void *stack_find_next_ptr(void **stack_top, void *stack_bottom, void *heap_start, void *heap_end)
{
  Dump_registers();
  /*
  unsigned long diff = ((unsigned long) *stack_top) - ((unsigned long) stack_bottom); 
  printf("top:  %lu\n", (unsigned long) *stack_top);
  printf("bot:  %lu\n", (unsigned long) stack_bottom);
  printf("dif:  %lu\n", diff);*/
  if(*stack_top < stack_bottom)
    {
      puts("smaller");
      while(*stack_top != stack_bottom)
        {
          void *result = *stack_top;
          if( *(unsigned long *)result <= (unsigned long)heap_end-sizeof(void *) 
            && *(unsigned long *)result >= (unsigned long)heap_start)
            {
              ++*stack_top;
              return result;
            }
          ++*stack_top;
        }
    }
  else
    {    
      puts("larger");
      while(*stack_top != stack_bottom)
        {
          void *result = *stack_top;
          if( *(unsigned long *)result <= (unsigned long)heap_end-sizeof(void *) 
            && *(unsigned long *)result >= (unsigned long)heap_start)
            {
              --*stack_top;
              return result;
            }
          --*stack_top;
        }
    }
  return NULL;
}


