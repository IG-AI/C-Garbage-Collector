/**
 *  @file  header.h
 *  @brief A module for creating headers (meta data) for data and structures.
 *
 *  @author Daniel Agstrand
 *  @author Henrik Bergendal
 *  @author Adam Inersjo
 *  @author Maria Lindqvist
 *  @author Simon Pellgard
 *  @author Robert Rosborg
 */

#include <stdlib.h>
         
#ifndef __header__
#define __header__

/**
 *  @brief Creates a header and saves it on the heap
 *
 *  @param format_string
 *  @param heap_ptr the place on the heap where the header will be saved
 */
void create_struct_header(char *format_string, void *heap_ptr);


/**
 *  @brief Creates a header and saves it on the heap
 *
 *  @param bytes the size in bytes
 *  @param heap_ptr the place on the heap where the header will be saved
 */
void create_data_header(size_t bytes, void *heap_ptr);


#endif
