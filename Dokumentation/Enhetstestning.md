#Enhetstestning inkl coverage
###Körning av tester
För att köra alla enhetstester på linux x86 och solaris x86 används "make test". 
Vi har totalt 182 tester vilka alla passerar.  

För att köra de 109 header-testerna används "make test_header" 

För att köra de 68 gc-testerna används "make test_gc"

För att köra de 3 alloc_map-testerna används "make test_alloc_map" 

För att köra de 2 stack-search-testerna används "make test_stack_search" 

På solaris SPARC används "make test_sparc" och 3 test fallerar.

###Coverage
För att se code coverage används "make coverage". Detta testar coverage för våra moduler när deras test-filer körs. Vi kunde dock bara få det att fungera för stack-search och alloc-map modulerna. Vi lyckades som sagt inte få det att fungera för header.c och gc.c på terminalerna i 1515, däremot kunde Tobias få resultat på sin egen dator.

Coverage för gc.c: Lines executed:93.72% of 366 <br/>
[Function coverage gc.c](gc_c_cov_f.txt)

Coverage för header.c: Lines executed:92.86% of 280 <br/>
[Function coverage header.c](header_c_cov_f.txt)

Coverage för stack_search.c: Lines executed:100.00% of 10 <br/>
[Function coverage stack_search.c](stack_search_c_cov_f.txt)

Coverage för alloc_map.c: Lines executed:100.00% of 29 <br/>
[Function coverage alloc_map.c](alloc_map_c_cov_f.txt)
