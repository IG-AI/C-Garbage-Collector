#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "header.h"
#include "header_hidden.h"

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


#define PTR '*'
#define INT 'i'
#define CHAR 'c'
#define LONG 'l'
#define FLOAT 'f'
#define DOUBLE 'd'

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

size_t
get_struct_size(char *form_str)
{
  if (form_str == NULL || *form_str == '\0') return INVALID;
  
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
          if (multiplier == INVALID) return INVALID;
          i += parsed_chars - 1;
          form_str += parsed_chars - 1;
        }
      else
        {
          size_t current_size = size_for(current);
          if(current_size == INVALID) return INVALID;
            
          if (multiplier == 0)
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
  if (multiplier != 0) result += multiplier * CHAR_SIZE;
  return result + HEADER_SIZE;
}
