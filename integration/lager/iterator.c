#include <stdlib.h>
#include "list.h"


#ifdef GC
#include "../../gc.h"
extern heap_t *heap;
#endif

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
#ifdef GC
  iter *result = h_alloc_struct("*");
#else
  iter *result = malloc(sizeof(*result));
#endif
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
#ifdef GC
  return;
#else
  free(iter);
#endif
}

