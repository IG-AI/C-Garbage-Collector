GC_FILES =  ../../gc.o ../../header.o ../../alloc_map.o ../../stack_search.o


tree.o: tree.c
	gcc -Wall -std=gnu11 -ggdb -c tree.c

%.o: %.c
	gcc -Wall -std=gnu11 -pedantic -ggdb -c $<

db: db.o tui.o tree.o list.o iterator.o undo.o
	gcc -Wall -std=gnu11 -pedantic -ggdb -o db db.o tui.o tree.o list.o iterator.o undo.o

dbgc: db.c tui.c tree.c list.c iterator.c undo.c

	gcc -Wall -std=gnu11 -pedantic -ggdb -DGC -o db $^ $(GC_FILES)

testgc: dbgc
	cat ./start.txt - | valgrind --leak-check=full ./db

gc_perf_test:
	gcc -Wall -std=gnu11 -pedantic -O1 gc_perf_test.c list.c iterator.c -o gc_perf_test -DGC $(GC_FILES)

tree_test: tree.o tree_test.o
	gcc -Wall -std=gnu11 -o tree_test tree_test.o tree.o

test: tree_test db
	#valgrind --leak-check=full ./db
	cat ./start.txt - | valgrind --leak-check=full ./db

run_perf: gc_perf_test
	@echo "Running 6 times, discount best and worst case"
	@echo "------ 1 ---------------------"
	time ./gc_perf_test 10000 10000
	@echo "------ 2 ---------------------"
	time ./gc_perf_test 100000 10000 > /dev/null
	@echo "------ 3 ---------------------"
	time ./gc_perf_test 100000 10000 > /dev/null
	@echo "------ 4 ---------------------"
	time ./gc_perf_test 100000 10000 > /dev/null
	@echo "------ 5 ---------------------"
	time ./gc_perf_test 100000 10000 > /dev/null
	@echo "------ 6 ---------------------"
	time ./gc_perf_test 100000 10000 > /dev/null

clean:
	rm -f *.o gc_perf_test

