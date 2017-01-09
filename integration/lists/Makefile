TIME=/usr/bin/time

ioopm: list_bench.c utils.c ../../all.o
	gcc -std=c11 -Wall -O3 -DIOOPM_GC list_bench.c utils.c ../../all.o -o ioopm

bdw: list_bench.c utils.c
	@echo "This will not work unless BDW is installed on your machine!"
	gcc -std=c11 -Wall -O3 -DBOEHM_GC list_bench.c utils.c -lgc -o bdw

nogc: list_bench.c utils.c
	gcc -std=c11 -Wall -O3 -DNO_GC list_bench.c utils.c -o nogc

clean:
	rm -f ioopm bdw nogc *.o

bench: nogc ioopm # bdw 
	$(TIME) --verbose -o ioopm.txt ./ioopm
#	$(TIME) --verbose -o bdw.txt ./bdw
	$(TIME) --verbose -o nogc.txt ./nogc
