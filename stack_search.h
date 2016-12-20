/**
 *  @file  stack_search.h
 *  @brief A module for searching through the stack.
 *
 *  @author Daniel Agstrand
 *  @author Henrik Bergendal
 *  @author Adam Inersjo
 *  @author Maria Lindqvist
 *  @author Simon Pellgard
 *  @author Robert Rosborg
 */

#ifndef __stack_search__
#define __stack_search__

/**
 *  @brief Finds a possible pointer on the stack.
 *
 *
 *  @param  stack_bottom the bottom of the stack to seach
 *  @param  stack_top the top of the stack to seach
 *  @param  heap_start the start of the heap
 *  @param  heap_end the last position in the heap to which a pointer is 
 *          allowed to refer
 *  @return a possible pointer into the heap, or NULL if search is finished
 *
 *  When calling the function use;
 *  
 *  Before calling stack search registers need to be dumped to the stack.
 *  #include <setjmp.h>
 *  #define Dump_registers() \
 *  jmp_buf env; \
 *  if (setjmp(env)) abort(); \
 *
 *  __builtin_frame_address(n);
 *  To get a pointer to the start of a scopes
 *  memory on stack. <n> specifies what scope. 0 is the current, 1 the one above.
 *  
 *  extern char **environ;
 *  To get the bottom of the programs' stack. When refering to it use something
 *  similar to:
 *  void *stack_bottom = (void *)*environ;
 *  void *pointer = stack_find_next_ptr(&stack_bottom, stack_top, heap_start, heap_end);
 *
 *  
 */
void **stack_find_next_ptr(void **stack_bottom, void *stack_top,
                           void *heap_start, void *heap_end);


#endif
