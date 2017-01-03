#Stack search

- [Introduktion](#introduktion)
- [Sökning i stacken](#sökning-i-stacken)
 - [Adressers positioner](#adressers-positioner)


##Introduktion
För att kunna göra en skräpsammlare så begöver man hitta rötter in i heapen på stacken. Detta görs genom att titta på alla positioner på stacken som kan innehålla pekare och jämföra dessa med heapens adressrymd.

##Sökning i stacken

Sökningen i stacken sker på det aktuella programmets stack, genom upprepat användande av följande kod:

__Search for pointers__
```c
void **stack_find_next_ptr(void **stack_bottom, void *stack_top, void *heap_start, void *heap_end);
```

För att få stack top och bottom använder man följande kod:

__Get bottom of stack__
```c
extern char **environ;
void *stack_bottom = (void *)*environ;
```

__Get top of stack__
```c
void *stack_top = __builtin_frame_address(0);
```

###Adressers positioner
Vi har gjort antagandet att adresser till heapen alltid ligger på adresser i stacken som är jämt delbara med åtta.
