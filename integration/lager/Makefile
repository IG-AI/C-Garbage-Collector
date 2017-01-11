GC_FILES =  ../../garbage_collector.o 

ifeq ($(PLATFORM), sparc)
  CC =cc
  STD =-std=c11
  LINKFLAGS =$(STD) -g -xmemalign=1i
  COMPFLAGS =$(STD) -g -c -m64 -xmemalign=1i -ggdb
  TESTFLAGS =$(STD) -g -m64 -lcunit -xmemalign=1i -DNDEBUG -pg
  COVERAGEFLAGS =$(STD) -Wall -g -m64 -lcunit -xmemalign=1i -DNDEBUG -fprofile-arcs -ftest-coverage -coverage
else
  CC =gcc
  STD =-std=gnu11
  LINKFLAGS =$(STD) -Wall -g -o -pg
  COMPFLAGS =$(STD) -Wall -g -c -m64 -ggdb -pg
  TESTFLAGS =$(STD) -Wall -g -m64 -lcunit -DNDEBUG -pg
  COVERAGEFLAGS =$(STD) -Wall -g -m64 -lcunit -DNDEBUG -fprofile-arcs -ftest-coverage -coverage
endif


tree.o: tree.c
	$(CC) $(COMPFLAGS) tree.c

%.o: %.c
	$(CC) $(COMPFLAGS) $<

db: db.o tui.o tree.o list.o iterator.o undo.o
	$(CC) $(LINKFLAGS) -o db db.o tui.o tree.o list.o iterator.o undo.o

dbgc: db.c tui.c tree.c list.c iterator.c undo.c
	$(CC) $(TESTFLAGS) -DGC -o db $^ $(GC_FILES)

dbgc_sparc:
	@$(MAKE) --no-print-directory dbgc PLATFORM=sparc

testgc: dbgc
	cat ./start.txt - | valgrind --leak-check=full ./db

gc_perf_test:
	$(CC) $(TESTFLAGS) -O1 gc_perf_test.c list.c iterator.c -o gc_perf_test -DGC -DGC_RUN $(GC_FILES)

gc_perf_test_run:
	$(CC) $(TESTFLAGS) -O1 gc_perf_test.c list.c iterator.c -o gc_perf_test -DGC -DGC_RUN $(GC_FILES)

no_gc_perf_test:
	$(CC) $(TESTFLAGS) -O1 gc_perf_test.c list.c iterator.c -o no_gc_perf_test

tree_test: tree.o tree_test.o
	$(CC) $(LINKFLAGS) -o tree_test tree_test.o tree.o

test: tree_test db
	#valgrind --leak-check=full ./db
	cat ./start.txt - | valgrind --leak-check=full ./db

run_perf: gc_perf_test
	@echo "Running 6 times, discount best and worst case"
	@echo "------ 1 ---------------------"
	time ./gc_perf_test 30000 10000 > /dev/null
	@echo "------ 2 ---------------------"
	time ./gc_perf_test 10000 10000 > /dev/null
	@echo "------ 3 ---------------------"
	time ./gc_perf_test 10000 10000 > /dev/null
	@echo "------ 4 ---------------------"
	time ./gc_perf_test 10000 10000 > /dev/null
	@echo "------ 5 ---------------------"
	time ./gc_perf_test 10000 10000 > /dev/null
	@echo "------ 6 ---------------------"
	time ./gc_perf_test 10000 10000 > /dev/null


# Performance Tests
run_db_fit: clean run_perf_fit_nogc clean run_perf_fit_gc clean

run_perf_fit_nogc: no_gc_perf_test
	@echo "\nRunning Big program that fits:"
	@echo "---- Using Malloc:"
	time ./no_gc_perf_test 100000 0

run_perf_fit_gc: gc_perf_test
	@echo "---- Using GC:"
	time ./gc_perf_test 100000 0

run_db_no_fit: clean run_perf_no_fit_nogc clean run_perf_no_fit_gc clean

run_perf_no_fit_nogc: no_gc_perf_test
	@echo "\nRunning Big program that fits:"
	@echo "---- Using Malloc:"
	time ./no_gc_perf_test 100000 10000 

run_perf_no_fit_gc: gc_perf_test
	@echo "---- Using GC:"
	time ./gc_perf_test 100000 10000





clean:
	rm -f *.o gc_perf_test no_gc_perf_test

