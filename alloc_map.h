/**
 *  @file  alloc_map.h
 *  @brief A module for creating and handling the allocation map.
 *  Heavily based on bitmap provided by T. Wrigstad at:
 *  https://github.com/IOOPM-UU/ioopm16/blob/master/forelasningar/fas1/f12/f12.pdf
 *
 *  We have discovered that this implementation of allocation map require more
 *  memory than what should be needed. Instead of using one bit for each address
 *  it need one byte.
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
 *  This function creates an allocation map on the provided pointer.
 *
 *  @param alloc_map the adress where the allocation map is to be created.
 *  
 *  @param start_addr the starting address for the memory block the
 *         allocation map represents.
 *  @param word_size the size of the words which the memory block is
 *         divided into.
 *  @param block_size the size in bytes of the memory block the map is
 *         going to represent
 *  
 */
void 
alloc_map_create(alloc_map_t *alloc_map, void *start_addr, size_t word_size, size_t block_size);

/**
 *  @brief Gets the size needed for an allocation map.
 *
 *  @param word_size the size of the words which the memory block is
 *         divided into.
 *
 *  @param block_size the size in bytes of the memory block the map is
 *         going to represent.
 *         
 *  @return the size in bytes needed to store the allocation map.
 */
size_t
alloc_map_mem_size_needed(size_t word_size, size_t block_size);

/**
 *  @brief Looks up if a memory address is pointing to the start of an object
 *         allocated on the gc-heap.
 *
 *  @param alloc_map pointer to the alloc map.
 *  @param ptr pointer to the adress to look up.
 *
 *  @return A bool representing the bit of the supplied ptr. False if @p ptr not
 *          in scope of @p alloc_map
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
