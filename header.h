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

/**
 *  @brief The enumeration of different types a header can have.
 *
 *  
 */
enum header_type {
  RAW_DATA          /**< The header is followed by raw data */
  , STRUCT_REP      /**< The header is followed by data containing pointers */
  , FORWARDING_ADDR /**< The header contains information about forwarded data */
  , NOTHING         /**< There is no header */
};

typedef enum header_type header_type;

/**
 *  @brief Creates a header and saves it on the heap
 *
 *  This function is called when the header has to contain information
 *  about pointers inside of data.
 *  The header created will be of the type STRUCT_REP
 *  The @p format_string will be copied and doesn't need to be placed
 *  on the heap before hand.
 *  
 *  @param  format_string the string representation of the structure to
 *          create a header for
 *  @param  heap_ptr the place on the heap where the header will be saved
 *  @return pointer to where the data should be placed
 */
void *create_struct_header(char *format_string, void *heap_ptr);

/**
 *  @brief Creates a header and saves it on the heap
 *
 *  This function is called when the header doesn't have to contain information
 *  about pointers existing inside data.
 *  The header created will be of the type RAW_DATA
 *  
 *  @param  bytes the size in bytes
 *  @param  heap_ptr the place on the heap where the header will be saved
 *  @return pointer to where the data should be placed
 *          NULL if @p bytes is zero or @p heap_ptr is NULL
 */
void *create_data_header(size_t bytes, void *heap_ptr);

/**
 *  @brief Gets the type of header belonging to the data.
 *
 *  The available types are FORWARDING_ADDR, STRUCT_REP and RAW_DATA.
 *  
 *  @param  data the data to analyze
 *  @return the type of header beloning to @p data, NOTHING if
 *          @p data is NULL
 */
header_type get_header_type(void *);

/**
 *  @brief Get the number of pointers inside @p structure.
 *
 *  The function will only return valid information if a struct_header has been
 *  created and placed just before the struct in the heap.
 *  
 *
 *  @param  structure the structure to look for pointers in
 *  @return number of pointers found in @p structure if its header has the type
 *          STRUCT_REP. Otherwise it will return -1
 */
int get_number_of_pointers_in_struct(void *structure);

/**
 *  @brief Finds all pointers inside @p structure and places pointers to
 *         them in @p array.
 *
 *  The function will only return valid information if a struct_header has been
 *  created and placed just before the struct in the heap.
 *  
 *  @p structure has to have the header type Structure_rep.
 *  @p array must be of size get_numer_of_pointers_in_structure(@p structure).
 *
 *  @param  structure the structure to look for pointers is
 *  @param  array the array to place pointers to the pointers found in header of
 *          @p structure
 *  @return true if pointers were found and placed in @p array, false otherwise
 */
bool get_pointers_in_struct(void *structure, void **array[]);

/**
 *  @brief Calculates the size needed to store the structure represented
 *         by @p format_string, including a header.
 *
 *  @param  format_string the string representation of a structure
 *  @return the size needed for the structure
 */
size_t get_struct_size(char *format_string);

/**
 *  @brief Calculates the size needed to store data of size @p bytes
 *  with a header.
 *
 *  @param  bytes the size of the data
 *  @return the size needed for storing the data with a header. If @p bytes
 *          combined with the size of the header extends maximum size
 *          for size_t the function will return 0. Zero is also returned
 *          when @p bytes is 0.
 */
size_t get_data_size(size_t bytes);

/**
 *  @brief Get the size of data having a header
 *
 *  The size of the header itself is included in the result
 *
 *  @param  data pointer to the data to get size of
 *  @return size of data combined with header
 */
size_t get_existing_size(void *data);

/**
 *  @brief Creates a copy of a header and saves the copy on the heap
 *
 *  
 *  @param  data the address to data containing the header to be copied 
 *  @param  heap_ptr the place on the heap where the header will be saved
 *  @return pointer to where the data should be placed
 *          NULL if @p data is NULL or @p heap_ptr is NULL
 */
void *copy_header(void *data , void *heap_ptr);


#endif
