/**
 *  @file  alloc_map.h
 *  @brief A module for creating and handling the allocation map.
 *
 *  @author Daniel Agstrand
 *  @author Henrik Bergendal
 *  @author Adam Inersjo
 *  @author Maria Lindqvist
 *  @author Simon Pellgard
 *  @author Robert Rosborg
 */

#include <stdbool.h>
         
#ifndef __alloc_map__
#define __alloc_map__
/**
 *  The alloc map is a collection of boolean values each showing if 
 *  an adress on the heap is at the start of an object.
 *
 */
typedef struct alloc_map alloc_map_t;

/**
 *  @brief Creates allocation map.
 *
 *  This function creates an allocation map, and returns the pointer to its location.
 *  
 *  @param start_addr the starting address for the memory block the allocation map represents.
 *  @param word_size the size of the words which the memory block is divided into.
 *  @param number_of_words how many words the memory block is, also the number of
 *         bools needed for the allocation map.
 */
alloc_map_t *alloc_map_create(void *start_addr, int word_size, int number_of_words);

/**
 *  @brief Looks up if a memory address is pointing to the start of an object
 *         allocated on the gc-heap.
 *
 *  @param ptr pointer to the adress to look up.         
 */
bool alloc_map_ptr_is_used(void *ptr);

/**
 *  @brief Flags an address as used.
 *
 *  @param ptr the pointer to set.
 *  @param state the value to set.
 */  
bool alloc_map_set_ptr(void *ptr, bool state);


/**
 *  @brief Frees the memory used for the allocation map.
 */
bool alloc_map_free();

#endif
