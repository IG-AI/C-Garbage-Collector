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
#include <stdbool.h>
         
#ifndef __header__
#define __header__

// TODO: define a header_type enum.

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

/**
 *  @brief Gets the type of header belonging to the structure.
 *
 *  @param  structure the structure to analyze
 *  @return the type of header beloning to @p structure
 */
header_type get_header_type(void *structure);

/**
 *  @brief Get the number of pointers inside @p structure.
 *
 *  @param  structure the structure to look for pointers in
 *  @return number of pointer found in @p structure
 */
int get_number_of_pointers_in_structure(void *structure);

/**
 *  @brief Finds all pointers inside @p structure and places them in @p array.
 *
 *  @p array must be of size get_numer_of_pointers_in_structure(@p structure).
 *
 *  @param  structure the structure to look for pointers is
 *  @param  array the array to place the pointers found in
 *  @return true if pointers were found and placed in @p array, false otherwise
 */
bool get_pointers_in_structure(void *structure, void *array[]);

/**
 *  @brief Calculates the size of the structure represented by @p format_string.
 *
 *  @param  format_string the string representation of a structure
 *  @return the size needed for the structure
 */
size_t get_data_size(char *format_string);

#endif
