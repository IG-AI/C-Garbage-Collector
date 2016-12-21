#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "header.h"
#include "header_hidden.h"

#define _XOPEN_SOURCE 500
extern char *strdup(const char *s);

#define PTR    '*'
#define INT    'i'
#define CHAR   'c'
#define LONG   'l'
#define FLOAT  'f'
#define DOUBLE 'd'

enum internal_ht
  {
    I_HT_NOTHING
    , I_HT_RAW_DATA
    , I_HT_FORWARDING_ADDR
    , I_HT_FORMAT_STR
    , I_HT_BIT_VECTOR
  };

typedef enum internal_ht internal_ht;


#define B_FORMAT_STR 0UL
#define B_FORWARDING_ADDR 1UL
#define B_RAW_DATA 2UL
#define B_BIT_VECTOR 3UL


/*============================================================================
 *                             HELPER FUNCTIONS
 *===========================================================================*/

/**
 *  @brief Gets the type size of a char
 *
 *  Valid chars are '*', 'i', 'c', 'l', 'f' & 'd'
 *
 *  @param  c char to get type size of
 *  @return the size of the type of @p c. INVALID if c isn't a type
 */
size_t
size_for(char c)
{
  if      (c == PTR)    return PTR_SIZE;
  else if (c == INT)    return INT_SIZE;
  else if (c == CHAR)   return CHAR_SIZE;
  else if (c == LONG)   return LONG_SIZE;
  else if (c == FLOAT)  return FLOAT_SIZE;
  else if (c == DOUBLE) return DOUBLE_SIZE;
  else                  return INVALID;
}

/**
 *  @brief Duplicate a string
 *
 *  The duplicate is currently allocated on the regular heap but will be changed
 *  to be allocated on our own heap in gc.c
 *
 *  @param  str the string to duplicate
 *  @return a copy of @p str allocated on the heap
 */
char *
str_duplicate(char *str)
{
  /*
    Should allocate on our heap, waiting on completed h_alloc_data.
    How to get the heap here...?
  */
  return strdup(str);
}

/**
 *  @brief Extract pointer to a header from a pointer to data.
 *
 *  @param  data pointer to the data to get the header from
 *  @return pointer to the header belonging to @p data
 */
void *
header_from_data(void *data)
{
  return (char *) data - HEADER_SIZE;
}

/**
 *  @brief Extract pointer to data from a pointer to a header.
 *
 *  @param  header pointer to the header to get the data from
 *  @return pointer to the data belonging to @p header
 */
void *
data_from_header(void *header)
{
  return (char *) header + HEADER_SIZE;
}

/**
 *  @brief Gives a copy of a header with type bits cleared.
 *
 *  @param  header the header to clear type bits from
 *  @return a copy of @p header with type bits set to 00
 */
void *
clear_type_bits(void *header)
{
  unsigned long header_u = (unsigned long ) header;
  return (void *) ((header_u >> 2UL) << 2UL); // CROSS_PLATFORM
}

/**
 *  @brief Set the type bits of a header to a specific type
 *
 *  @param  header_ptr pointer to the header to set type bits on
 *  @param  type the type to set to @p header_ptr
 */
void
set_type_bits(void *header_ptr, internal_ht type)
{
  if(type == I_HT_NOTHING) return;

  unsigned long type_bits;
  if(type == I_HT_RAW_DATA)             type_bits = B_RAW_DATA;
  else if(type == I_HT_FORMAT_STR)      type_bits = B_FORMAT_STR;
  else if(type == I_HT_FORWARDING_ADDR) type_bits = B_FORWARDING_ADDR;
  else                                  type_bits = B_BIT_VECTOR;
  
  unsigned long header_u = (unsigned long) clear_type_bits(*(void **)header_ptr);
  *(void **)header_ptr = (void *) (header_u | type_bits);
}


/*============================================================================
 *                             CREATION FUNCTIONS
 *===========================================================================*/


// The type RAW_DATA has '10'' as the last two bits
// The rest of the bits will containt the size of the data following the header
// The address returned is where the data will be stored
// Example:
//   create_data_header(4, 100)
//   will create a header with
//   ...000 100 10
//          ^^^ ^^
//           |   |
//           | binary for RAW_DATA
//           |
//        binary 4
//
//   return ptr will be 100 + HEADER_SIZE (108 in this case)
void *
create_data_header(size_t bytes, void *ptr)
{
  if (bytes == 0 || ptr == NULL) return NULL;

  assert(sizeof(unsigned long) == sizeof(void *));
  unsigned long *ptr_to_header = (unsigned long *) ptr; // CROSS_PLATFORM
  *ptr_to_header = bytes << 2UL; // CROSS_PLATFORM
  set_type_bits(ptr_to_header, I_HT_RAW_DATA);
  return data_from_header(ptr);
}

void *
create_struct_header(char *form_str, void *ptr)
{
  if (form_str == NULL || ptr == NULL) return NULL;
  if (get_struct_size(form_str) == INVALID) return NULL;

  char **ptr_to_header = (char **) ptr;
  *ptr_to_header = str_duplicate(form_str);
  set_type_bits(ptr_to_header, I_HT_FORMAT_STR);
  return data_from_header(ptr);
}

/*============================================================================
 *                             TYPE FUNCTIONS
 *===========================================================================*/
/**
 *  @brief Get the internal type of a header
 *
 *  The internal types differ from the external types by having both FORMAT_STR
 *  and BIT_VECTOR as available types. These are known together as STRUCT_REP
 *  externally
 *
 *  @param  data the data to get header type of
 *  @return the header type of the header belonging to @p data
 */
internal_ht
get_internal_ht(void *data)
{
  if (data == NULL) return I_HT_NOTHING;
  void *header_ptr = header_from_data(data);
  unsigned long header = *(unsigned long *) header_ptr; // CROSS_PLATFORM
  unsigned long type_bits = header & 3UL; // CROSS_PLATFORM
  
  if(type_bits == B_FORMAT_STR) return I_HT_FORMAT_STR;
  else if(type_bits == B_BIT_VECTOR) return I_HT_BIT_VECTOR;
  else if(type_bits == B_RAW_DATA) return I_HT_RAW_DATA;
  else return I_HT_FORWARDING_ADDR;
}

header_type
get_header_type(void *data)
{
  internal_ht i_type = get_internal_ht(data);
  if(i_type == I_HT_FORMAT_STR || i_type == I_HT_BIT_VECTOR) return STRUCT_REP;
  else if(i_type == I_HT_RAW_DATA) return RAW_DATA;
  else if(i_type == I_HT_FORWARDING_ADDR) return FORWARDING_ADDR;
  else return NOTHING;
}


/*============================================================================
 *                             SIZE FUNCTIONS
 *===========================================================================*/

size_t
get_data_size(size_t bytes)
{
  if (bytes == 0 || bytes > SIZE_MAX - HEADER_SIZE)
    {
      return INVALID;
    }
  else
    {
      return bytes + HEADER_SIZE;
    }
}

#define BASE10 10

size_t
get_struct_size(char *form_str)
{
  if(form_str == NULL || *form_str == '\0') return INVALID;
  
  size_t result = 0;
  size_t multiplier = 0;
  char *current_ptr = form_str;
  while(*current_ptr != '\0')
    {
      char current = *current_ptr;
      if(isdigit(current) && current != '0')
        {
          char *after_number;
          multiplier = strtol(current_ptr, &after_number, BASE10);
          if(multiplier < 1) return INVALID;
          current_ptr = after_number;
        }
      else
        {
          size_t current_size = size_for(current);
          if(current_size == INVALID) return INVALID;
            
          if(multiplier == 0)
            {
              result += current_size;
            }
          else
            {
              result += multiplier * current_size;
              multiplier = 0;
            }
          ++current_ptr;
        }
    }
  if(multiplier != 0) result += multiplier * CHAR_SIZE;
  return result + HEADER_SIZE;
}

/** Get the size of an existing data of type RAW_DATA */
size_t
get_raw_data_size(void *data)
{
  if(get_internal_ht(data) != I_HT_RAW_DATA) return INVALID;

  unsigned long *header = header_from_data(data);
  unsigned long size = (unsigned long) *header >> 2UL;
  return get_data_size(size);
}

/** Get the size of an existing data of type I_HT_FORMAT_STR */
size_t
get_format_str_size(void *structure)
{
  if(get_internal_ht(structure) != I_HT_FORMAT_STR) return INVALID;

  char **header = header_from_data(structure);
  return get_struct_size(*header);
}

size_t
get_existing_size(void *ptr)
{
  if(ptr == NULL) return INVALID;

  internal_ht type = get_internal_ht(ptr);
  if(type == I_HT_FORWARDING_ADDR) return INVALID;
  else if(type == I_HT_RAW_DATA) return get_raw_data_size(ptr);
  else if(type == I_HT_FORMAT_STR) return get_format_str_size(ptr);
  assert(false && "Bit vector not yet implemented");
  return INVALID;
}


/*============================================================================
 *                             Getting pointers functions
 *===========================================================================*/

int
number_of_pointers_in_format_str(char *str)
{
  int result = 0;
  int multiplier = 0;
  char *current_ptr = str;
  while(*current_ptr != '\0')
    {
      char current = *current_ptr;
      if(isdigit(current))
        {
          char *after_num;
          multiplier = strtol(current_ptr, &after_num, BASE10);
          current_ptr = after_num;
        }
      else
        {
          if(current == PTR && multiplier == 0) ++result;
          else if(current == PTR) result += multiplier;
          multiplier = 0;
          ++current_ptr;
        }
    }
  return result;
}

int
get_number_of_pointers_in_struct(void *structure)
{
  if(structure == NULL || get_header_type(structure) != STRUCT_REP) return -1;

  void *format_str_ptr = header_from_data(structure);
  char *format_str = *((char **) format_str_ptr);
  return 1 + number_of_pointers_in_format_str(format_str);
}

/** Moves a pointer forward by @p ammount */
void *
move_ptr_forward(void *ptr, size_t ammount)
{
  unsigned long new_data = (unsigned long) ptr + ammount;
  return (void *) new_data;

}

void
get_pointers_from_num(char **ptr_to_str
                      , void **array[]
                      , size_t *array_index
                      , void **current_data)
{
  char *current_ptr = *ptr_to_str;
  char *after_num;
  int num = strtol(current_ptr, &after_num, BASE10);
  current_ptr = after_num;
  char current = *current_ptr;
  if(current == PTR)
    {
      for(int j = 0; j < num; ++j)
        {
          array[*array_index] = *current_data;
          ++*array_index;
          *current_data = move_ptr_forward(*current_data, PTR_SIZE);
        }
    }
  else
    {
      assert(size_for(current) != INVALID);
      size_t ammount = size_for(current) * num;
      *current_data = move_ptr_forward(*current_data, ammount);
    }
  *ptr_to_str = current_ptr;

}

bool
get_pointers_in_struct(void *structure, void **array[])
{
  if(structure == NULL || array == NULL) return false;
  if(get_header_type(structure) != STRUCT_REP) return false;
  assert(get_number_of_pointers_in_struct(structure) > 0);
  
  array[0] = header_from_data(structure); // Implement for bit-vector
  char *current_ptr = *array[0];
  size_t array_index = 1;
  void *current_data = structure;
  while(*current_ptr != '\0')
    {
      char current = *current_ptr;
      if(isdigit(current))
        {
          get_pointers_from_num(&current_ptr, array, &array_index, &current_data);
        }
      else
        {
          if(current == PTR)
            {
              array[array_index] = current_data;
              ++array_index;
            }
          ++current_ptr;
          assert(size_for(current) != INVALID);
          current_data = move_ptr_forward(current_data, size_for(current));
        }
     }

  
  return true;
}


/*============================================================================
 *                             Forwarding and copying
 *===========================================================================*/

void *
copy_header(void *data , void *heap_ptr)
{
  if (data == NULL) return NULL;
  if (heap_ptr == NULL) return NULL;

  *(void **)heap_ptr = *(void **)header_from_data(data);

  return data_from_header(heap_ptr);
}


bool
forward_header(void *data, void *new_data)
{
  if (data == NULL) return false;
  if (new_data == NULL) return false;
  if (data == new_data) return false;

  void **header_ptr = header_from_data(data);
  *header_ptr = new_data;
  set_type_bits(header_ptr, I_HT_FORWARDING_ADDR);
  
  return true;
}

void *
get_forwarding_address(void *data)
{
  if (data == NULL) return NULL;
  if (get_header_type(data) != FORWARDING_ADDR) return NULL;

  void **header_ptr = header_from_data(data);
  return clear_type_bits(*header_ptr);
}

/*============================================================================
 *                             TEST HELPING FUNCTIONS
 *===========================================================================*/
int
additional_if_format_str(void *data)
{
  if(get_internal_ht(data) == I_HT_FORMAT_STR) return 1;
  else return 0;
}
