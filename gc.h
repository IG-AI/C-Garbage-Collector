/**
 *  @file  gc.h
 *  @brief A library for heap allocation with automatic garbage collection.
 *
 *  @author Daniel Agstrand
 *  @author Henrik Bergendal
 *  @author Adam Inersjo
 *  @author Maria Lindqvist
 *  @author Simon Pellgard
 *  @author Robert Rosborg
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef __gc__
#define __gc__

#define UNSAFE_STACK true
#define SAFE_STACK false

/**
 *  @brief The opaque data type holding all the heap data.
 */
typedef struct heap heap_t;


/**
 *  @brief Create a new heap with @p bytes total size.
 *
 *  The size available for allocation will be strictly less than @p bytes
 *  because of metadata and other internal data.
 *
 *  @param  bytes the total size of the heap in bytes
 *  @param  unsafe_stack true if pointers on the stack are to be
 *          considered unsafe pointers
 *  @param  gc_threshold the memory pressure at which gc should be triggered
 *          (1.0 = full memory)
 *  @return the new heap or NULL if memory cannot be allocated
 */
heap_t *
h_init(size_t bytes, bool unsafe_stack, float gc_threshold);


/**
 *  @brief Delete a heap.
 *
 *  @param h the heap
 */
void 
h_delete(heap_t *h);


/**
 *  @brief Delete a heap and trace, killing off stack pointers.
 *
 *  Only valid pointers will be replaced. A valid pointer is a pointer
 *  with the address of an allocated space in @p h.
 *
 *  @param h the heap
 *  @param dbg_value a value to be written into every valid pointer into @p h
 *         on the stack
 */
void 
h_delete_dbg(heap_t *h, void *dbg_value);


/**
 *  @brief Allocate a new object on a heap with a given format string.
 *
 *  Valid characters in format strings are:
 *  - 'i' -- for sizeof(int) bytes 'raw' data
 *  - 'l' -- for sizeof(long) bytes 'raw' data
 *  - 'f' -- for sizeof(float) bytes 'raw' data
 *  - 'c' -- for sizeof(char) bytes 'raw' data
 *  - 'd' -- for sizeof(double) bytes 'raw' data
 *  - '*' -- for a sizeof(void *) bytes pointer value
 *  - '\0' -- null-character terminates the format string
 *
 *  @param  h the heap
 *  @param  layout the format string
 *  @return the newly allocated object
 *  
 *  @note   the heap does *not* retain an alias to layout.
 */
void *
h_alloc_struct(heap_t *h, char *layout);


/**
 *  @brief Allocate a new object on a heap with a given size.
 *
 *  Objects allocated with this function will *not* be
 *  further traced for pointers.
 *
 *  @param  h the heap
 *  @param  bytes the size in bytes
 *  @return the newly allocated object
 */
void *
h_alloc_data(heap_t *h, size_t bytes);


/**
 *  @brief Manually trigger garbage collection.
 *
 *  Garbage collection is otherwise run when an allocation is
 *  impossible in the available consecutive free memory.
 *
 *  @param  h the heap
 *  @return the number of bytes collected
 */
size_t 
h_gc(heap_t *h);



/**
 *  @brief Manually trigger garbage collection with the ability to
 *         override the setting for how stack pointers are treated.
 *
 *  Garbage collection is otherwise run when an allocation is
 *  impossible in the available consecutive free memory.
 *
 *  @param  h the heap
 *  @param  unsafe_stack true if pointers on the stack are to be
 *          considered unsafe pointers
 *  @return the number of bytes collected
 */
size_t 
h_gc_dbg(heap_t *h, bool unsafe_stack);



/**
 *  @brief Returns the available free memory.
 *
 *  @param  h the heap
 *  @return the available free memory.
 */
size_t 
h_avail(heap_t *h);



/**
 *  @brief Returns the bytes currently in use in a heap.
 *
 *  This includes any meta-data specific to user allocated structures
 *  and any internal padding.
 *
 *  @param  h the heap
 *  @return the bytes currently in use by user structures.
 */
size_t 
h_used(heap_t *h);



/**
 *  @brief Return the byte  size of the heap.
 *  
 *  @param h the heap
 *  @return the byte size of the heap.
 */
size_t 
h_size(heap_t *h);

#endif
