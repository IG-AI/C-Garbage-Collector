MOD=./modules/
CC= gcc
FLAGS= -c -g -Wall -std=gnu99 -lcunit #-coverage
#SOURCES=lager_utils.c utils.c tree.c list.c list_utils.c ask.c undo.c
#OBJECTS=$(SOURCES:.c=.o)

#gc: lager.o $(OBJECTS)
#	$(CC) -g lager.o $(OBJECTS) -o $@

stack_search.o: stack_search.c
	$(CC) $(FLAGS) stack_search.c -o $@


stack_search_test: stack_search.o stack_search_test.o
	$(CC) -g  stack_search.o stack_search_test.o -o $@ -lcunit	-fprofile-arcs -ftest-coverage
	valgrind ./stack_search_test

stack_search_test.o: stack_search_test.c
	$(CC) $(FLAGS) stack_search_test.c -o $@ -lcunit


.PHONY: clean
clean:
	rm -f *.o
	rm -f *.gcno
	rm -f *.gcda 
	rm -f *.gcov
	rm -f stack_test
