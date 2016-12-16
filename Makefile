CC=gcc
FLAGS=-std=c11  -Wall -Werror


all: gc

gc: gc.c
	$(CC) $(FLAGS) $^ -o $@

run: gc
	./gc


test_gc: tester_gc
	./tester_gc

tester_gc: gc_test.c gc.o
	$(CC) $(FLAGS) $^ -o $@ -lcunit


memtester_gc: gc_test.c gc.o
	$(CC) $(FLAGS) $^ -o $@ -lcunit


memtest_gc: memtester_gc
	valgrind -v --leak-check=full ./memtester


gc.o: gc.c gc.h
	$(CC) $(FLAGS) -c $^

clean_gc:
	rm -f gc
	rm -f tester
	rm -f memtester
	rm -f gc.h.gch
	rm -f *.o
	rm -rf *.dSYM
