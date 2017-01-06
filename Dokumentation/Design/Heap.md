#Heapen

## Innehåll
- [Konservativ kompaktering efter Bartlet](#Konservativ kompaktering efter Bartlett)

###Konservativ kompaktering efter Bartlett
Bartlett skiljer mellan säkra och osäkra pekare. En säker pekare är en adress
som vi säkert vet är en pekare. En osäker pekare är en vars data vi inte säkert
vet är en pekare. Ett typiskt användande av Bartletts teknik är för skanning av
stacken i ett C-liknande språk, där vi inte vet vad det är för data vi tittar på.
Vi klassificerar alltså alla pekare vi hittar som säkra eller osäkra. Att vara
konservativ innebär att vi måste utgå från att en osäker pekare faktiskt är en
pekare i bemärkelsen att vi måste betrakta dess utpekade objekt som levande,
och samtidigt att vi måste utgå från att den osäkra pekaren faktiskt inte är
en pekare vilket innebär att vi inte kan flytta dess utpekade objekt i minnet
eftersom det kräver att vi ändrar pekarvärdet till den nya adressen. Exempel,
om vi hittar 0x39C5F6 (slutpriset på en lägenhet) på stacken måste det objekt
som ligger på den adressen överleva och inte flyttas. Flyttade vi det till t.ex.
adressen 0x1C0030 måste vi uppdatera värdet på stacken till 0x1C0030 (peka
om ”pekaren”), vilket skulle betyda att vi ändrat slutpriset på lägenheten!
För att använda Bartletts trick för att hantera osäkerhet delar vi in minnet
som vi hanterar i ett antal diskreta ”sidor” 13 . Istället för att dela in hela heapen
i två delar – passiv och aktiv – ger vi varje sida statusen passiv eller aktiv. En
osäker pekare till en adress A medför nu att den omslutande sidan P inte får
flyttas vid kompaktering.

###Höga adresser
Detta är enkelt att implementera och brukar ge hög avkastning. Använd t.ex.
posix_memalign för att allokera minnet till programmets egen heap och ange
en mycket hög adress som alignment. Det medför att alla pekaradresser som
skapas kommer att vara väldigt stora. Eftersom program sällan manipulerar
väldigt stora tal minskar risken för att ett heltal i programmet skulle råka
sammanfalla med en valid minnesadress.

###Att skapa och riva ned en heap
Funktionen h_init som ni skall implementera skapar en ny heap med en
angiven storlek och returnerar en pekare till den. Utöver storlek skall det gå att
ställa in två ytterligare parametrar:
* Huruvida pekare på stacken skall anses som säkra eller osäkra
* Vid vilket minnestryck skräpsamling skall köras

Eftersom det måste vara möjligt att resonera om minneskraven för en
applikation skall allt metadata om heapen också rymmas i det angivna stor-
leksutrymmet. Dvs., alla eventuella kopior av formatsträngar etc. skall lagras
i “er heap”. Funktionen h_avail returnerar antalet tillgängliga bytes i en heap,
dvs. så många bytes som kan allokeras innan minnet är fullt (dvs. minnestryc-
ket är 100%).
Det skall finnas två funktioner för att frigöra en heap och återställa allt
minne:
* h_delete som frigör allt minne som heapen använder.
* h_delete_dbg som utöver ovanstående också ersätter alla variabler på

stacken som pekar in i heapens adressrymd med ett angivet värde, t.ex. NULL
eller 0xDEADBEEF så att ”skjutna pekare” (eng. dangling pointers) lättare
kan upptäckas.

###Gränssnittet gc.h
Nedanstående headerfil sammanfattar det publika gränssnitt som skall imple-
menteras. En doxygen-dokumenterad version finns också tillgänglig i kursens
repo.
```c
#include <stddef.h>
#include <stdbool.h>

#ifndef __gc__
#define __gc__

typedef struct heap heap_t;

heap_t *h_init(size_t bytes, bool unsafe_stack, float gc_threshold);
void h_delete(heap_t *h);
void h_delete_dbg(heap_t *h, void *dbg_value);

void *h_alloc_struct(heap_t *h, char *layout);
void *h_alloc_raw(heap_t *h, size_t bytes);

size_t h_avail(heap_t *h);
size_t h_used(heap_t *h);
size_t h_gc(heap_t *h);
size_t h_gc_dbg(heap_t *h, bool unsafe_stack);
#endif
```
