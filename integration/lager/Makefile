GC_FILES =  ../../gc.o ../../header.o ../../alloc_map.o ../../stack_search.o


tree.o: tree.c
	gcc -Wall -std=gnu11 -ggdb -c tree.c -pg

%.o: %.c
	gcc -Wall -std=gnu11 -pedantic -ggdb -c $< -pg

db: db.o tui.o tree.o list.o iterator.o undo.o
	gcc -Wall -std=gnu11 -pedantic -ggdb -o db db.o tui.o tree.o list.o iterator.o undo.o -pg

dbgc: db.c tui.c tree.c list.c iterator.c undo.c

	gcc -Wall -std=gnu11 -pedantic -ggdb -DGC -o db $^ $(GC_FILES) -pg

testgc: dbgc
	cat ./start.txt - | valgrind --leak-check=full ./db -pg

gc_perf_test:
	gcc -Wall -std=gnu11 -pedantic -O1 gc_perf_test.c list.c iterator.c -pg -o gc_perf_test -DGC $(GC_FILES) 

gc_perf_test_original:
	gcc -Wall -std=gnu11 -pedantic -O1 gc_perf_test.c list.c iterator.c -pg -o gc_perf_test

tree_test: tree.o tree_test.o
	gcc -Wall -std=gnu11 -o tree_test tree_test.o tree.o -pg

gc_all: 
	cd ../../ && make all 


test: tree_test db
	#valgrind --leak-check=full ./db
	cat ./start.txt - | valgrind --leak-check=full ./db

run_perf: gc_perf_test
	@echo "Running 6 times, discount best and worst case"
	@echo "------ 1 ---------------------"
	time ./gc_perf_test 20000 20000 > /dev/null
	@echo "------ 2 ---------------------"
	time ./gc_perf_test 20000 20000 > /dev/null
	@echo "------ 3 ---------------------"
	time ./gc_perf_test 20000 20000 > /dev/null
	@echo "------ 4 ---------------------"
	time ./gc_perf_test 20000 20000 > /dev/null
	@echo "------ 5 ---------------------"
	time ./gc_perf_test 20000 20000 > /dev/null
	@echo "------ 6 ---------------------"
	time ./gc_perf_test 20000 20000 > /dev/null

clean:
	rm -f *.o gc_perf_test

