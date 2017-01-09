PLATFORM ?=x86

ifeq ($(PLATFORM), sparc)
  CC =cc
  STD =-std=c11
  LINKFLAGS =$(STD) -g -o -xmemalign=1i
  COMPFLAGS =$(STD) -g -c -m64 -xmemalign=1i
  TESTFLAGS =$(STD) -g -m64 -lcunit -xmemalign=1i -DNDEBUG
  COVERAGEFLAGS =$(STD) -Wall -g -m64 -lcunit -xmemalign=1i -DNDEBUG -fprofile-arcs -ftest-coverage -coverage
else
  CC =gcc
  STD =-std=c11
  LINKFLAGS =$(STD) -Wall -g -o
  COMPFLAGS =$(STD) -Wall -g -c -m64 -pg
  TESTFLAGS =$(STD) -Wall -g -m64 -lcunit -DNDEBUG -pg
  COVERAGEFLAGS =$(STD) -Wall -g -m64 -lcunit -DNDEBUG -fprofile-arcs -ftest-coverage -coverage
endif

test_all: gc.o stack_search.o header.o alloc_map.o
	ld -r gc.o stack_search.o header.o alloc_map.o -o all.o


all:	gc.o stack_search.o header.o alloc_map.o 
	@echo "Compiled"

gc.o: gc.c gc.h #header.o stack_search.o alloc_map.o
	@$(CC) $(COMPFLAGS) gc.c -o $@

header.o: header.c header.h
	@$(CC) $(COMPFLAGS) header.c -o $@

stack_search.o: stack_search.c stack_search.h
	@$(CC) $(COMPFLAGS) stack_search.c -o $@

alloc_map.o: alloc_map.c alloc_map.h
	@$(CC) $(COMPFLAGS) alloc_map.c -o $@



# TESTS
test: gc_test header_test stack_search_test alloc_map_test
	@echo "*************************************************************"
	@echo "GC tests:"
	@./gc_test

	@echo "*************************************************************"
	@echo "Header tests:"
	@./header_test

	@echo "*************************************************************"
	@echo "Stack-search tests:"
	@./stack_search_test

	@echo "*************************************************************"
	@echo "Allocation map tests:"
	@./alloc_map_test

	@echo "Completed"

coverage: header_coverage stack_search_coverage alloc_map_coverage #gc_coverage
#	@./gc_coverage
#	@echo ""
#	@echo "GC coverage:"
#	gcov gc.c
#	@echo ""
#	@echo "*************************************************************"

	@./header_coverage
	@echo ""
	@echo "Header coverage:"
	@gcov header.c
	@echo ""
	@echo "*************************************************************"

	@./stack_search_coverage
	@echo ""
	@echo "Stack-search coverage:"
	@gcov stack_search.c
	@echo ""
	@echo "*************************************************************"

	@./alloc_map_coverage
	@echo ""
	@echo "Allocation-map coverage:"
	@gcov alloc_map.c
	@echo ""
	@echo "*************************************************************"

test_sparc:
	@$(MAKE) --no-print-directory PLATFORM=sparc test

# GC
test_gc: gc_test
	./gc_test

gc_test: gc_test.c gc.c header.o stack_search.o alloc_map.o
	@$(CC)  $^ -o $@ $(TESTFLAGS)

gc_coverage: gc.c gc_test.c header.c stack_search.c alloc_map.c
	@$(CC)  $^ -o $@ $(COVERAGEFLAGS) -lgcov

memtest_gc: gc_test
	valgrind --leak-check=full --log-file=memtest.txt  ./gc_test  #-v 


# Header
test_header: header_test
	@./header_test

header_test: header.o header_test.c
	@$(CC)  $^ -o $@ $(TESTFLAGS)

header_coverage: header.c header_test.c
	@$(CC)  $^ -o $@ $(COVERAGEFLAGS)

# Stack search
test_stack_search: stack_search_test
	@./stack_search_test

stack_search_test: stack_search.o stack_search_test.c 
	@$(CC)  $^ -o $@ $(TESTFLAGS)

stack_search_coverage: stack_search.c stack_search_test.c 
	@$(CC)  $^ -o $@ $(COVERAGEFLAGS)

# Alloc_map
test_alloc_map: alloc_map_test
	@./alloc_map_test

#THIS TEST HAVE TO RECOMPILE ALLOC_MAP.C WITH TESTFLAGS TO SUPRESS ASSERTS
alloc_map_test: alloc_map.c alloc_map_test.c
	@$(CC)  $^ -o $@  $(TESTFLAGS)

alloc_map_coverage: alloc_map.c alloc_map_test.c
	@$(CC)  $^ -o $@  $(COVERAGEFLAGS)

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
	@rm -f gc_coverage
	@rm -f gc.h.gch
	@rm -rf *.dSYM
	@echo "GC-files cleared"

clean_header:
	@rm -f header_test
	@rm -f header_coverage
	@echo "Header files cleared"

clean_stack_search:
	@rm -f stack_search_test
	@rm -f stack_search_coverage
	@echo "Stack Search files cleared"

clean_alloc_map:
	@rm -f alloc_map_test
	@rm -f alloc_map_coverage
	@echo "Alloc map files cleared"
