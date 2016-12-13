#include <stdint.h>

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


size_t
get_struct_size(char *format_string)
{
  return 0;
}
