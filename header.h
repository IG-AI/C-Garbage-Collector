
#ifndef __header__
#define __header__

/// creates a header and saves it on the heap
/// \param format_string
/// \param heap_ptr the place on the heap where the header will be saved
void create_struct_header(char *format_string, void* heap_ptr);


/// creates a header and saves it on the heap
/// \param bytes the size in bytes
/// \param heap_ptr the place on the heap where the header will be saved
void create_data_header(size_t bytes, void* heap_ptr);


#endif
