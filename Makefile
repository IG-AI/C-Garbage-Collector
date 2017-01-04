SOURCES= stack_search.c
OBJECTS=$(SOURCES:.c=.o)

CC =gcc
STD =-std=c11
LINKFLAGS =$(STD) -g -o
COMPFLAGS =$(STD) -Wall -g -c -m64
TESTFLAGS =$(STD) -Wall -g -m64 -lcunit  #-fprofile-arcs -ftest-coverage -coverage

all: gc

gc: gc.o stack_search.o header.o alloc_map.o
	$(CC) $(LINKFLAGS) $@ $^

gc.o: gc.c gc.h
	$(CC) $(COMPFLAGS) -c $^

header.o: header.c header.h
	@$(CC) $(COMPFLAGS) header.c

stack_search.o: stack_search.c stack_search.h
	$(CC) $(COMPFLAGS) stack_search.c -o $@

alloc_map.o: alloc_map.c alloc_map.h
	$(CC) $(COMPFLAGS) alloc_map.c -o $@



# TESTS
test: test_gc test_header test_stack_search test_alloc_map
	@echo "Completed"

# GC
test_gc: gc_test
	./gc_test

gc_test: gc_test.c gc.o header.o stack_search.o alloc_map.o
	@$(CC)  $^ -o $@ $(TESTFLAGS)

memtest_gc: gc_test
	valgrind -v --leak-check=full ./gc_test


# Header
test_header: header_test
	@./header_test

header_test: header.o header_test.c
	@$(CC)  $^ -o $@ $(TESTFLAGS)


# Stack search
test_stack_search: stack_search_test
	@./stack_search_test

stack_search_test: stack_search.o stack_search_test.c 
	@$(CC)  $^ -o $@ $(TESTFLAGS)



# Alloc_map
test_alloc_map: alloc_map_test
	@./alloc_map_test

alloc_map_test: alloc_map_during_test.o alloc_map_test.c
	$(CC)  $^ -o $@  $(TESTFLAGS)

alloc_map_during_test.o: alloc_map.c alloc_map.h
	$(CC) $(COMPFLAGS) alloc_map.c -o $@ -DNDEBUG


# CLEANUP
.PHONY: clean
clean: clean_header clean_gc clean_stack_search clean_alloc_map
	@rm -f *.o
	@rm -f *.gcno
	@rm -f *.gcda 
	@rm -f *.gcov
	@echo "All files cleared"

clean_gc:
	@rm -f gc
	@rm -f gc_test
	@rm -f gc.h.gch
	@rm -rf *.dSYM
	@echo "GC-files cleared"

clean_header:
	@rm -f header_test
	@echo "Header files cleared"

clean_stack_search:
	@rm -f stack_search_test
	@echo "Stack Search files cleared"

clean_alloc_map:
	@rm -f alloc_map_test
	@echo "Alloc map files cleared"
