#Allokeringskarta

##Innehåll
- [Introduktion](#introduktion)
- [Implementation](#implementation)
- [Reflektion](#reflektion)

##Introduktion
För att minska risken av felaktiga pekarvärden från stacken så använde vi en allokeringskarta.
Tanken var att kartan skulle vara en bit-karta där varje giltig adress på heapen representerades av en bit.
Använda adresser markeras med true, oanvända med false.

##Implementation
För initialisering tar allokeringskartan en startadress, en objektstorlek samt ett maximalt antal objekt.
Alla adresser är markerade som oanvända (false) efter att allokeringskartan skapats.

__Create new allocation map__
```c
// Create an allocation map for an array of 256 integers
int *start_addr = malloc(256*sizeof(int));
alloc_map_t *alloc_map = alloc_map_create(start_addr, sizeof(int), 256*sizeof(int));
```
När vi vill markera att en adress används sätter vi dess värde till antingen true eller false.

__Set a bit in the allocation map__
```c
// Set the n:th bit in the allocation map to true
alloc_map_set(alloc_map, &start_addr[n], true);

// Set the m:th bit in the allocation map to false
alloc_map_set(alloc_map, &start_addr[m], false);
```
För att se om en adress används anropar vi modulen med addressen vi är intresserade av. 

__Get value of bit in allocation map__
```c
// Find out the value of the n:th bit
alloc_map_ptr_used(alloc_map, &(start_addr[n]));
```
##Reflektion
Vi upptänkte efter tester att även om vår allokeringskarta fungerade någorlunda korrekt så verkar det inte vara en bit-karta utan snarare en byte-karta. Detta gör att allokeringskartan använder betydligt mer plats (åtta gånger så mycket) än vad den hade behövt göra. Exakt varför detta uppstått och hur vi ska rätta till det har vi inte hunnit undersöka närmre, för tillfället finner vi det viktigare att vi har en fungerande allokeringskarta än att den använder mindre minne.
