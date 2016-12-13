CC =gcc
STD =-std=c11
LINKFLAGS =$(STD) -g -o
COMPFLAGS =$(STD) -Wall -g -c


header.o: header.c header.h
	@$(CC) $(COMPFLAGS) header.c

header_test.o: header_test.c
	@$(CC) $(COMPFLAGS) header_test.c

test_header: header.o header_test.o
	@$(CC) $(LINKFLAGS) header_test header.o header_test.o -lcunit
	@./header_test



clean:
	@rm -f *.o header_test	
