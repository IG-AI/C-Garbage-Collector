#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "header.h"
#include "header_hidden.h"

#define _XOPEN_SOURCE 500
extern char *strdup(const char *s);

#define PTR '*'
#define INT 'i'
#define CHAR 'c'
#define LONG 'l'
#define FLOAT 'f'
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

#define ASCII_ZERO 48
#define ASCII_NINE 57
#define ASCII_OFFSET 48
#define INVALID_NUM -1

/**
 *  @brief Turn a numerical char into an int
 *
 *  
 *
 *  @param  c the char to convert
 *  @return the int corresponding to @p c, INVALID_NUM if c isn't a digit
 */
int
char_to_digit(char c)
{
  if(c < ASCII_ZERO || c > ASCII_NINE)
    {
      return INVALID_NUM;
    }
  else
    {
      return c - ASCII_OFFSET;
    }
}

/**
 *  @brief Parses a positive number from a string.
 *
 *  
 *
 *  @param  str the string to parse a positive number from
 *  @param  parsed_chars pointer to where to stor number of chars parsed
 *  @return a number parsed from @p str.
 *          INVALID if no positive number was able to be parsed from @p str,
 */
size_t
parse_number(char *str, size_t *parsed_chars)
{
  // To Optimize: use "strtol"
  if (str == NULL || parsed_chars == NULL) return INVALID;
  size_t result = 0;
  size_t i = 0;
  char current = str[i];

  while (current != '\0' && isdigit(current))
    {
      int number = char_to_digit(current);
      assert(current > 0);
      if (result == 0 && number == 0)
        {
          return INVALID;
        }
      else 
        {
          result = result * 10;
          result += number;
          ++i;
          current = str[i];
        }
    }
  *parsed_chars = i;
  return result;
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

size_t
get_struct_size(char *form_str)
{
  if(form_str == NULL || *form_str == '\0') return INVALID;
  
  size_t result = 0;
  size_t multiplier = 0;
  int str_len = strlen(form_str);
  for (int i = 0; i < str_len; ++i, form_str += 1)
    {
      char current = *form_str;
      if(isdigit(current))
        {
          size_t parsed_chars;
          multiplier = parse_number(form_str, &parsed_chars);
          if(multiplier == INVALID) return INVALID;
          i += parsed_chars - 1;
          form_str += parsed_chars - 1;
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
  for(int i = 0; i < (int) strlen(str); ++i)
    {
      if(isdigit(str[i]))
        {
          size_t parsed_chars;
          multiplier = parse_number(&str[i], &parsed_chars);
          i += parsed_chars - 1;
        }
      else
        {
          if(str[i] == PTR && multiplier == 0) ++result;
          else if(str[i] == PTR) result += multiplier;
          multiplier = 0;
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
