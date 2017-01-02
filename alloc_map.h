/**
 *  @file  alloc_map.h
 *  @brief A module for creating and handling the allocation map.
 *  Heavily based on bitmap provided by T. Wrigstad at:
 *  https://github.com/IOOPM-UU/ioopm16/blob/master/forelasningar/fas1/f12/f12.pdf
 *
 *  @author Daniel Agstrand
 *  @author Henrik Bergendal
 *  @author Adam Inersjo
 *  @author Maria Lindqvist
 *  @author Simon Pellgard
 *  @author Robert Rosborg
 */

#ifndef __alloc_map__
#define __alloc_map__
#include <stdbool.h>
#include <stdlib.h>


       
/**
 *  The alloc map is a collection of boolean values each showing if 
 *  an adress on the heap is at the start of an object.
 *
 */
typedef struct alloc_map alloc_map_t;

/**
 *  @brief Creates allocation map.
 *
 *  This function creates an allocation map, and returns the pointer to
 *  its location.
 *  
 *  @param start_addr the starting address for the memory block the
 *         allocation map represents.
 *  @param word_size the size of the words which the memory block is
 *         divided into.
 *  @param block_size the size in bytes of the memory block the map is
 *         going ot represent
 *
 *  @return pointer to the newly created allocation map.
 *  
 */
alloc_map_t *
alloc_map_create(void *start_addr, size_t word_size, size_t block_size);



/**
 *  @brief Looks up if a memory address is pointing to the start of an object
 *         allocated on the gc-heap.
 *
 *  @param alloc_map pointer to the alloc map.
 *  @param ptr pointer to the adress to look up.
 *
 *  @return A bool representing the bit of the supplied ptr.
 */
bool 
alloc_map_ptr_used(alloc_map_t *alloc_map, void *ptr);



/**
 *  @brief Flags an address.
 *
 *  @param alloc_map pointer to the alloc map  
 *  @param ptr the pointer to set.
 *  @param state the value to set.
 *
 */  
bool 
alloc_map_set(alloc_map_t *alloc_map, void *ptr, bool state);



#endif
