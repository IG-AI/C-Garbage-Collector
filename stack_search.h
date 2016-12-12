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
 *  @param  stack_top the top of the stack to seach
 *  @param  stack_bottom the bottom of the stack to seach
 *  @param  heap_start the start of the heap
 *  @param  heap_end the end of the heap
 *  @return a possible pointer into the heap
 */
void **stack_find_next_ptr(void **stack_top, void *stack_bottom,
                          void *heap_start, void *heap_end);


#endif
