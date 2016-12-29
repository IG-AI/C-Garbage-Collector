# Header modulen

Den här filen innehåller designplanen för Header modulen. Kom ihåg att detta är
ett levande dokument och kommer att ändras. 

## Innehåll

- [Introduktion](#introduktion)
- [Metadata](#metadata)
  - [Skapande av meta-data](#skapande-av-metadata)
  - [Formatsträngar](#formatsträngar)
  - [Bitvektorer](#bitvektorer)
- [Beräkning av storlek](#beräkning-av-storlek)
- [Objektpekare pekar förbi headern](#objektpekare-pekar-förbi-headern)
- [Funktionalitet som inte implementerats än](#funktionalitet-som-inte-implementerats-än)
- [Exempel](#exempel)

## Introduktion
Header modulen kommer hantera allt som har med meta-data och formatsträngar att
göra. Följande funktionalitet kommer att finnas:

* Beräkning av storlek som behövs för att få plats med data på heapen
  , inklusive meta-data
* Beräkning av storlek för existerande data, inklusive meta-data
* Skapande av meta-data, innehållande representation av structs
* Ändrande av meta-data, vid forwarding av data
* Checkning av vilken typ av meta-data som data har
* Extrahering av pekare från structar
* Kopiering av meta-data

## Metadata
Varje allokering på heapen behöver metadata för att kunna hålla koll på storlek
och pekare. Denna metadata läggs precis före datat som den tillbehör.

Metadatat består av 8 bytes och har fyra olika typer. Typerna anges med hjälp
av de två minst signifikanta bitarna i metadatat.

| Mönster | Headern är en ...                |
|--------:|----------------------------------|
| 00      | Pekare till en formatsträng      |
| 01      | Forwarding-address               |
| 10      | Storleksanvisning för 'ren data' |
| 11      | Bitvektor                        |

#### Typ 00
Detta innebär att de resterande 62 bitarna i metadatat är en address till en
[formatsträng](#formatsträngar).

#### Typ 01
Denna header är en forwarding-address som pekar på var headerns data har flyttats
till. För att använda addressen "tvättas" typ-bitarna bort.

#### Typ 10
Denna headers resterande 62 bitar är ett positivt tal som står för hur stort det
efterföljande datat är.

#### Typ 11
De resterande 62 bitarna i denna header är en [bitvektor](#bitvektorer) som representerar
storlek och uppbyggnad av dess efterföljande data.


## Skapande av metadata
Externt finns det två sätt att skapa meta-data: med en [formatsträng](#formatsträngar)
eller med en storleksangivelse.
Internt finns det dock en tredje representation för meta-data: [bitvektorer](#bitvektorer).

När meta-data skapas med hjälp av en formatsträng som är tillräckligt kort 
(se [bitvektorer](#bitvektorer)) används en bitvektor i meta-datan. Om formatsträngen
inte är kort kommer denna att allokeras på heapen.

Oavsätt vilken meta-data som skapas behövs plats för denna allokeras, (se 
[Beräkning av storlek](#beräkning-av-storlek))

## Formatsträngar
En formatsträng är en sträng som representerar hur data som allokeras är uppbyggt.

Formatsträngen förklaras enklast genom exempel. Antag att vi har en typ
`binary_tree_node`, deklarerad enligt följande.
```c
struct binary_tree_node {
    void *value;
    struct binary_tree_node *left;
    struct binary_tree_node *right;
    int balanceFactor;
}
```
Detta utrymme kan beskrivas av formatsträngen ”\*\*\*i” som betyder att utrymme
skall allokeras för 3 pekare, följt av en int, dvs.,
`alloc("***i");`
är analogt med
`alloc(3 * sizeof(void *) + sizeof(int));`


Följande styrkoder skall kunna ingå i en formatsträng:

| Symbol | Betydelse | Storlek\* |
|:--:|-----------|---------------|
| \* | pekare    | 8 bytes       |
| c  | char      | 1 byte        |
| i  | int       | 4 bytes       |
| l  | long      | 8 bytes       |
| f  | float     | 4 bytes       |
| d  | double    | 8 bytes       |

(\* på en 64-bitars plattform)

Ett heltal före ett specialtecken avser repetition; till exempel är
"\*\*\*ii" ekvivalent med "3\*2i". Man kan se det som att default-värdet inte
måste sättas ut explicit, alltså * är kortform för 1*. En tom formatsträng är
inte valid. Inte heller formatsträngar vars heltal börjar med 0 är valida.
En formatsträng som bara innehåller ett heltal, t.ex. "32", tolkas som "32c".
Detta innebär att `h_alloc_struct("32")` är semantiskt ekvivalent med
`h_alloc_raw(32)`.

För att formatsträngen ska kunna hållas "levande" i heapen ges pekaren till denna
ut från funktionen `get_pointers_in_struct()`. Därmed kan skräpsamlaren flytta
formatsträngen så att den inte räknas som skräp.

## Bitvektorer
En mer kompakt representation av en formatsträng skapas om antalet tecken är färre
än 31. (Undantag finns för `long` och `double`, dessa kan bara vara 15 st)
Denna representation finns för att effektivisera heapen för mindre structar.

Bitvektorn börjar vid MSB och består av par av bitar med olika betydelser.
Programmet läser av dessa bitar två och två och tolkar dem som deras motsvarighet
i formatsträngar.

Bessa bit-par har följande betydelser:

| Bitar | Storlek | Används för |
|:--:|-----------|------------|
| 00 | 0 bytes   | STOP       |
| 01 | 1 byte    | char       |
| 10 | 4 bytes   | int, float |
| 11 | 8 bytes   | pekare     |

00 betyder alltså att det inte finns någon mer information i bitvektorn.

11 används bara för pekare då det indikerar för programmet att det finns pekare
på en specifik plats. På grund av detta kan 11 inte användas för annan data som
är 8 bitar (long, double). Dessa representeras istället som två int/float, dvs. 1010.
Detta är anledningen till att bitvektorn endast kan representera 15 st long/double.

## Beräkning av storlek
Beräkning av storlek kommer ske vid två tillfällen i programmet:

* Innan data allokerats på heapen
* När data ska kopieras till ny del av heapen (vid skräpsamling)

#### Innan data allokerats
Innan data allokerats på heapen ska `get_data_size()` eller `get_struct_size()`
anropas för att få reda på hur mycket plats som behövs på heapen.

`get_data_size` tar endast en storleksanvisning i bytes som argument och kommer
returnera hur mycket plats som behövs för datan med den storleken tillsammans med
headern.

`get_struct_size` tar en formatsträng (se nedan) och beräknar hur mycket plats
structen som strängen representerar behöver (inklusive header).

#### När data ska kopieras
För att kunna kopiera data från en "page" till en annan i heapen måste man veta hur
mycket plats som behövs. Denna information kan man få ut från data som har en
header genom att anropa `get_existing_size`. Funktionen ger en storlek som inkluderar
meta-data.


## Objektpekare pekar förbi headern
Objektets header ligger alltid __först__ i objektet, men __skall inte vara
synlig i några struktar__ (det skulle göra programmet beroende av en specifik
skräpsamlare, vilket vore dåligt). Därför kommer en pekare till ett objekt
alltid att peka på struktens första byte, dvs. den __”pekar förbi”__ headern.
Och om man vill komma åt headern måste man använda pekararitmetik och ”backa”
`sizeof(header)` bytes. Denna typ av design tillåter att skräpsamlaren modiferas
så att headern växer och krymper utan att program som använder skräpsamlaren
måste modiferas.


## Funktionalitet som inte implementerats än
- Om en formatsträng inte innehåller pekare ska det skapas en header av typ RAW_DATA

## Exempel
__Creation of Data Header__
```c
// Get the size needed for header + data
size_t size = get_data_size( sizeof(int) );
// Get space for both header and data
void *start_ptr = calloc(1, size);
// Create the header
void *data_ptr = create_data_header( sizeof(int), start_ptr);
```
__Creation of Struct Header__
```c
// Get the size needed for header + struct
size_t size = get_struct_size("*i2l");
// Get space for both header and struct
void *start_ptr = calloc(1, size);
// Create the header
void *struct_ptr = create_struct_header( "*i2l", start_ptr);
```
__Extract pointers from Struct__
```c
// Get the ammount of pointers inside of the struct
int num_ptrs = get_number_of_pointers_in_struct(sturct_ptr); // From above
assert(num_ptrs != -1);

// Create array to put pointers to heap pointers in
void **ptr_array[num_ptrs];
bool success = get_pointers_in_struct(struct_ptr, ptr_array);
// Continue working with the pointers...
```
