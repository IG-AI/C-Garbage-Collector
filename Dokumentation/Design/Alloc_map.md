#Allokeringskarta

##Innehåll
-[Introduktion](#introduktion)

##Introduktion
Ett sätt att minska risken för felaktiga pekarvärden är att använda
en allokeringskarta. En allokeringskarta är en array av booleans där varje plats
i arrayen motsvarar en valid adress för en allokering, och där true betyder
att något allokerats på den platsen (false – inte). Om man t.ex. har (vilket är
rimligt) en minsta objektstorlek på 16 bytes behöver man alltså en array med
1024 element för att hålla reda på 16 kb. Om man använder en bitvektor där
en enskild bit är en boolean behövs alltså bara 128 bytes för att hålla reda på 16
kb, vilket är <1% overhead.

##Implementation
Allokeringskartan tar en startadress, en objektstorlek samt ett maximalt antal objekt.

__Create new allocation map__
```c
// Create an allocation map for an array of 256 integers
int *start_addr = malloc(256*sizeof(int));
alloc_map_t *alloc_map = alloc_map_create(start_addr, sizeof(int), 256*sizeof(int));
```
__Set a bit in the allocation map__
```c
// Set the n:th bit in the allocation map to true
alloc_map_set(alloc_map, &start_addr[n], true);

// Set the m:th bit in the allocation map to false
alloc_map_set(alloc_map, &start_addr[m], false);
```
__Get value of bit in allocation map__
```c
// Find out the value of the n:th bit
alloc_map_ptr_used(alloc_map, &(start_addr[n]);
```
