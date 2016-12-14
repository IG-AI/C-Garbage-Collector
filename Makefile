CC=gcc
FLAGS=-std=c11  -Wall -Werror


all: gc

gc: gc.c
	$(CC) $(FLAGS) $^ -o $@

run: gc
	./gc


test: tester
	./tester

tester: gc_test.c gc.o
	$(CC) $(FLAGS) $^ -o $@ -lcunit


memtester: gc_test.c gc.o
	$(CC) $(FLAGS) $^ -o $@ -lcunit


memtest: memtester
	valgrind -v --leak-check=full ./memtester


gc.o: gc.c gc.h
	$(CC) $(FLAGS) -c $^

clean:
	rm -f gc
	rm -f tester
	rm -f memtester
	rm -f gc.h.gch
	rm -f *.o
	rm -rf *.dSYM
