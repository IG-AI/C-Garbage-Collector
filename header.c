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

char *
str_duplicate(char *str)
{
  /*
    Should allocate on our heap, waiting on completed h_alloc_data.
    How to get the heap here...?
  */
  return strdup(str);
}

void *
header_from_data(void *ptr)
{
  return (char *) ptr - HEADER_SIZE;
}

void *
data_from_header(void *ptr)
{
  return (char *) ptr + HEADER_SIZE;
}


/*============================================================================
 *                             CREATION FUNCTIONS
 *===========================================================================*/

#define B_FORMAT_STR 0UL
#define B_FORWARDING_ADDR 1UL
#define B_RAW_DATA 2UL
#define B_BIT_VECTOR 3UL


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
  *ptr_to_header = (unsigned long) bytes << 2;          // CROSS_PLATFORM
  *ptr_to_header |= 2UL;                                // CROSS_PLATFORM
  return data_from_header(ptr);
}

void *
create_struct_header(char *form_str, void *ptr)
{
  if (form_str == NULL || ptr == NULL) return NULL;
  if (get_struct_size(form_str) == INVALID) return NULL;

  char **ptr_to_header = (char **) ptr;
  *ptr_to_header = str_duplicate(form_str);
  assert( ( (unsigned long) (*ptr_to_header) & 3UL) == B_FORMAT_STR); // C_P
  return data_from_header(ptr);
}

/*============================================================================
 *                             TYPE FUNCTIONS
 *===========================================================================*/
header_type
get_header_type(void *data)
{
  if (data == NULL) return NOTHING;
  void *header_ptr = header_from_data(data);
  unsigned long header = *(unsigned long *) header_ptr; // CROSS_PLATFORM
  unsigned long type_bits = header & 3UL; // CROSS_PLATFORM
  
  if(type_bits == B_FORMAT_STR || type_bits == B_BIT_VECTOR) return STRUCT_REP;
  else if(type_bits == B_RAW_DATA) return RAW_DATA;
  else return FORWARDING_ADDR;
}


/*============================================================================
 *                             SIZE FUNCTIONS
 *===========================================================================*/

size_t
get_data_size(size_t bytes)
{
  if (bytes == 0 || bytes > SIZE_MAX - HEADER_SIZE)
    {
      return 0;
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

size_t
get_existing_size(void *ptr)
{
  // TODO
  return 0;
}


/*============================================================================
 *                             Getting pointers functions
 *===========================================================================*/

int
number_of_pointers_in_str(char *str)
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
  return 1 + number_of_pointers_in_str(format_str);
}

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
  
  array[0] = header_from_data(structure);
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
