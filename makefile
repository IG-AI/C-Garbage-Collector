MOD=./modules/
CC= gcc
FLAGS= -c -g -Wall -std=gnu99 -lcunit -m64 #-coverage
SOURCES= stack_search.c
OBJECTS=$(SOURCES:.c=.o)


stack_search.o: stack_search.c
	$(CC) $(FLAGS) stack_search.c -o $@

stack_search_test: stack_search.o stack_search_test.o 
	$(CC) -g  stack_search.o stack_search_test.o -o $@ -lcunit	-fprofile-arcs -ftest-coverage -m64
	./stack_search_test

stack_search_test.o: stack_search_test.c
	$(CC) $(FLAGS) stack_search_test.c -o $@ -lcunit


.PHONY: clean
clean:
	rm -f *.o
	rm -f *.gcno
	rm -f *.gcda 
	rm -f *.gcov
	rm -f stack_search_test
