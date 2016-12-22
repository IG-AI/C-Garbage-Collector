#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include "alloc_map.h"

#define Byte_index(siz,idx) (siz / idx)
#define Bit_index(siz,idx) (siz % idx)
#define On(a) 1UL << (a)
#define Off(a) ~(1UL << (a))
#define Index_check(a, b) assert(0 <= (a) && (a) < (b))


struct alloc_map 
{
  void * start_addr;
  size_t word_size;
  size_t map_size;
  uint8_t bits[]; // Weird C99 trick
};

alloc_map_t *alloc_map_create(void *start_addr, size_t word_size, size_t block_size)
{
  alloc_map_t *a = calloc(1, (block_size/word_size)+sizeof(size_t));
  a->start_addr = start_addr;
  a->word_size = word_size;
  a->map_size = (block_size/word_size);
  return a;
}

bool alloc_map_ptr_is_used(void *ptr)
{
  return false;
}

bool alloc_map_set_ptr(alloc_map_t *a, size_t size)
{
  Index_check(size, a->map_size / 4);
  a->bits[size / 4] |= On(size % 4);
}

void alloc_map_free(alloc_map_t * a)
{
  free(a);
}


    
bitset_t *bs_new(size_t siz)
{
  bitset_t *b = calloc(1, siz + sizeof(size_t));
  return b;
}
void bs_free(bitset_t *b)
{
  free(b);
}

bool bs_contains(bitset_t *b, size_t v)
{
  Index_check(v, b->size / 8);
  return b->bits[v / 8] & On(v % 8);
}
void bs_set(bitset_t *b, size_t v)
{
  Index_check(v, b->size / 8);
  b->bits[v / 8] |= On(v % 8);
}
void bs_unset(bitset_t *b, size_t v)
{
  Index_check(v, b->size / 8);
  b->bits[v / 8] &= Off(v % 8);
}
