#Heapen

## Innehåll
- [Introduktion](#introduktion)  
- [Heapen](#heapen)
 - [Deleta heapen](#deleta-heapen)
- [Allokering](#allokering)
- [Skräpsamlare](#skräpsamlare)
- [Debug versioner](#debug-versioner)
- [Gränssnittet gc.h](#gränssnittet-gch)

##Introduktion
För att kunna skapa en egen skräpsamlare behöver vi skapa en "egen heap" på heapen. Vi behöver även en egen allokeringsfunktion för att spara data på vår heap, samt en skräpsamlare för att automatiskt frigöra och kompaktera vår heap.  

##Heapen
När heapen skapas allokerar vi ett minnesblock på den riktiga heapen. Heapen delas upp i ett antal diskreta sidor, pages, med storlek 2048 bytes. Det måste minst finnas två sidor, vilket betyder att minsta storlek på heapen är 4096 bytes. Vid initiering av heapen skapas även en allokeringskarta. Vid skapande anges även en bool som anger om stacken är säker eller inte, samt ett tröskelvärde i procent för när skräpsamlingen ska aktiveras. 

Pages kan ha fyra olika värden, active, passive, transition och unsafe. Vid initering är alla pages passiva. Pekare till startadressen för varje sida läggs i en array i heap-strukten. Varje page har en page-bump som håller reda på var första lediga minnesplats börjar. 

###Deleta heapen
När heapen ska tas bort, vid h_delete, frigör vi allokeringskartan, minnesblocket och hela heapstructen. 


##Allokering
Det finns två funktioner för allokering, en för struktar (h_alloc_struct) och en för rå data (h_alloc_data).

Vid allokering söks första aktiva page upp. Det kollas om det finns tillräckligt ledigt minne för allokering på pagen genom att titta på page-bumpen. Om det finns ledigt minne allokeras datan/strukten på pagen och page-bumpen flyttas framåt. Annars letas nästa aktiva page upp. Om ingen aktiv page finns, sätts om möjligt den första passiva sidan till aktiv. Om det endast finns en passiv page kvar eller om vi går över tröskelvärdet för skräpsamling, körs skräpsamlaren och därefter testas igen om det det finns plats att allokera på. Om det fortfarande inte finns, returneras NULL. 

När en allokering har skett, sätts platsen på allokeringskartan motsvarande adressen som allokerats till True. 


##Skräpsamlare
När h_gc kallas, antingen manuellt eller genom att tröskelvärdet har överskridits, körs skräpsamlingen igång. Vi plockar ur stackens top och bottom, vilka används för att rekursivt hitta alla valida pekare på stacken och i heapen. Dessa läggs i en array. 

Alla aktiva pages sätts till transition. Den första passiva sidan sätts till active. Arrayen traverseras för varje transition-page, och alla objekt i den sidan som har pekare till sig, flyttas till en aktiv sida. När inga pekare längre pekar till sidan, sätts den till passiv och page-bumpen återställs.  

h_gc returnerar hur många bytes som har städats borts under skräpsamlingen. 


##Debug versioner
h_delete_dbg skriver över alla aktiva stack-pekare med ett givet värde. 

h_gc_dbg sätter stacken till unsafe, vilket innebär att alla pages som har en stack-pekare till sig sätts till unsafe. Dessa kan därmed inte ändras under skräpsamling. Efter skräpsamling sätts de tillbaka till active. 


##Gränssnittet gc.h

```c
typedef struct heap heap_t;

heap_t *
h_init(size_t bytes, bool unsafe_stack, float gc_threshold);

void 
h_delete(heap_t *h);

void 
h_delete_dbg(heap_t *h, void *dbg_value);

void *
h_alloc_struct(heap_t *h, char *layout);

void *
h_alloc_data(heap_t *h, size_t bytes);

size_t 
h_gc(heap_t *h);

size_t 
h_gc_dbg(heap_t *h, bool unsafe_stack);

size_t 
h_avail(heap_t *h);

size_t 
h_used(heap_t *h);

size_t 
h_size(heap_t *h);
```
