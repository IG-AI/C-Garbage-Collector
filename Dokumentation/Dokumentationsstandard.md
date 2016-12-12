# Documenting Standard

Here is an introduction to our documentation standard. It is not complete and
will be updated during the project. 

It is also a good idea to document functions inside the source file that aren't
exported in the header so that it is easier to understand the code.

### Module Documentation
Every module header (.h file) that should be included in the documentation needs
a file-specific comment block.
**Example:**
```c
/**
 *  @file  [Name of the file]
 *  @brief [Short description of the module, only one sentece]
 *
 *  [More in depth description of the module]
 *
 *  @author [Author 1]
 *  @author [Author 2]
 *  @date   [Last edited date]
 */
```

### Function Documentation
Every function in a header file should have a function-specific comment block.
**Example:**

```c
/**
 *  @brief Short description of the function, should be one sentence.
 *
 *  Longer description giving more information about what the function does.
 *  Use @p a to reference parameter a.
 *  An example can also be added by:
 *  @code
 *  int c = foo (2, 3);
 *  printf ("%d\n", c);
 *  @endcode
 *
 *  @param  a description of parameter a
 *  @param  b description of parameter b
 *  @return what the function returns
 *  @see    [Reference to Something]
 */
int
foo (int a, int b)
{
  return 0;
}
```

### Structs, Enums, Typedefs & Defines
All of these can use the same basic structure.
**Example:**
```c
/**
 *  @brief [Short desc of struct]
 *
 *  [More details]
 */
struct example
{
  int a;  /**< [Documentation for member example#a.] */
  int b;  /**< [Documentation for member example#b.] */
  char c; /**< [Documentation for member example#c.] */
};
```


### More information about commands and how to document
  
  - [General Information](http://www.stack.nl/~dimitri/doxygen/manual/docblocks.html)
  - [Available commands](http://www.stack.nl/~dimitri/doxygen/manual/commands.html)
