#include <stdlib.h>
#include "list.h"

extern heap_t *heap;

typedef struct node node;

struct iter
{
  node *current; 
};

/// Breaking encapsulation of the list required
/// for external iterator
struct node
{
  void *elem;
  node *next;
};

iter *iterator_new(void *current)
{
  iter *result = h_alloc_struct(heap, "*");
  result->current = (node *)current;
  return result;
}

void *iterator_next(iter *iter)
{
  if (iter->current)
    {
      void *element = iter->current->elem;
      iter->current = iter->current->next;
      return element;
    }
  else
    {
      return NULL;
    }
}

bool iterator_has_more(iter *iter)
{
  return iter->current != NULL;
}

void iterator_delete(iter *iter)
{
  free(iter);
}

