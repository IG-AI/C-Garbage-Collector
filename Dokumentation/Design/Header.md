# Header modulen

Den här filen innehåller designplanen för Header modulen. Kom ihåg att detta är
ett levande dokument och kommer att ändras. 

## Formatsträngar
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

__Notera__ att data alignment kan påverka en strukturs layout för mer effektiv
minnesåtkomst i en strukt. Detta kan betyda att två fält efter varandra i en
strukt har ”tomt utrymme” mellan sig för att värdens plats i minnet skall bättre
passa med ord-gränser. Man kan __antingen__ sätta sig in i hur detta fungerar –
notera att det är plattformsberoende – eller fundera ut hur man stänger av det.
Ange tydligt i dokumentationen hur detta har hanterats. 

En bra första iteration i implementationen av stödet för allokering med
formatsträng implementerar stöd för ”*” och ”r”, där det sistnämnda står för
`sizeof(int)`, vilket på en 64-bitars platform ger möjligheten att allokera
antingen i ”byggklossar” om 8 respektive 4 bytes 
(Faktiskt så räcker detta bra som en ”intern representation” av formatsträngen,
oavsett vad användaren skriver)

Åtminstone följande styrkoder skall kunna ingå i en formatsträng:

| Symbol | Betydelse |
|:--:|-----------|
| \* | pekare    |
| c  | char      |
| i  | int       |
| l  | long      |
| f  | float     |
| d  | double    |

Ett heltal före ett specialtecken avser __repetition__; till exempel är
"\*\*\*ii" ekvivalent med "3\*2i". Man kan se det som att default-värdet inte
måste sättas ut explicit, alltså * är kortform för 1*. __En tom formatsträng är
inte valid__. 
__En formatsträng som bara innehåller ett heltal, t.ex. "32", tolkas som"32c"__.
Detta innebär att `h_alloc_struct("32")` är semantiskt ekvivalent med
`h_alloc_raw(32)`.

## Implementationsdetaljer
Eftersom objekt i C inte har något metadata måste implementationen hålla reda på
två saker:

1. Hur stort varje objekt är (annars kan vi inte kopiera det), samt
2. Var i objektet dess pekare till andra objekt finns.

Formatsträngen innehåller information för att räkna ut båda dessa, men
formatsträngen är inte helt oproblematisk, t.ex. eftersom den ägs av klienten
som kan avallokera den. Om varje formatsträng kopierades med motsvarande strdup
skulle vi vara mer skyddade mot att de avallokeras i förtid, men det skulle bli
ett påtagligt slöseri att skapa många kopior av strängar. En bättre
implementation skulle använda en mer kompakt representation av formatsträngen
som kunde bakas in i det allokerade objektet. (Detta leder också till färre
minnesaccesser, vilket är bra för prestandan.)

I implementationen av den kompakterande skräpsamlaren skall varje objekt ges en
header (metadata), och vi är förstås intresserade av att denna header är __så
liten som möjligt__ eftersom program som allokerar många små objekt annars blir
för ineffektiva. En bra design är att spara headern precis före varje objekt i
minnet. Låt oss börja med att titta på vad headern skall kunna innehålla för
information:

1. En pekare till en formatsträng (const char *)
2. __En mer kompakt representation av objektets layout__
3. En forwarding-adress
4. En flagga som anger om objektet redan är överkopierat till den passiva arean
vid skräpsamling

Lyckligtvis kan vi representera samtliga dessa data i ett enda utrymme av
storlek `sizeof(void *)` med hjälp av litet klassisk C-slughet. Vi kan börja med
att notera att alternativ 1 och 2 är __ömsesigt uteslutande__, dvs., finns en
kompakt representation av objektets layout behövs varken formatsträng eller en
objektspecik skräpsamlingsfunktion, osv. Vidare behövs forwardingadress enbart n
är en __kopia redan har gjorts av objektet__, vilket t.ex. betyder att
forwarding-adressen kan skriva över objektets data eftersom allt överskrivet
data går att hitta om man bara följer forwarding-pekaren. Slutligen kan vi
konstatera att flaggan i 4 __enbart behövs i samband med 3__.

### Vilken information finns i headern
För att inte slösa med minnet skall vi använda de __två minst signifkanta
bitarna i en pekare__ för att koda in information om vad som nns i headern.
Alltså, om en 32-bitars pekare binärt är (med little-endian)
100100011110110001100101000001000 pratar vi om att gömma information i de sista
två, dvs. 1001000111101100011001010000010__.
(Notera skillnader mellan big-endian och little-endian i hur adresser
representeras binärt.)
Två bitar är tillräckligt för att koda in fyra olika tillstånd, t.ex.

| Mönster | Headern är en ... |
|--------:|-------------------|
| 00      | pekare till en formatsträng (alt. 1) |
| 01      | forwarding-adress (alt. 3)           |
| 10      | _tillgängligt för utökning_          |
| 11      | bitvektor med layoutinformation (alt 2. se nedan) |

Notera att de två minst signikanta bitarna måste __”maskas ut”__ ur pekaren
innan pekaren används – annars kan pekarvärde bli ogiltigt på grund av att datat
vi gömt där tolkas som en del av adressen. Det betyder att varje läsning av
headern som en pekare skall sätta de två minst signikanta bitarna till 0 i det
utlästa resultatet.

### Objektpekare pekar förbi headern
Objektets header ligger alltid __först__ i objektet, men __skall inte vara
synlig i några struktar__ (det skulle göra programmet beroende av en specifik
skräpsamlare, vilket vore dåligt). Därför kommer en pekare till ett objekt
alltid att peka på struktens första byte, dvs. den __”pekar förbi”__ headern.
Och om man vill komma åt headern måste man använda pekararitmetik och ”backa”
`sizeof(header)` bytes. Denna typ av design tillåter att skräpsamlaren modiferas
så att headern växer och krymper utan att program som använder skräpsamlaren
måste modiferas.

### En mer kompakt layoutspecifkation
Vi skall använda en __bitvektor__ för att koda in en layoutspecikation på ett
sätt som är betydligt mer yteektivt än en formatsträng. Vi kan t.ex. använda
en bit för att ange antingen en pekare eller ”data”, t.ex. 11001 är samma som
formatsträngen "\*\*ii\*", som ger en objektstorlek på 32 bytes om en pekare är
8 bytes och `sizeof(int)` är 4 bytes (8 + 8 + 4 + 4 + 8 = 32). Vi behöver också
information om __bitvektorns längd__. Notera att vi enbart är intresserade av
huruvida något är en pekare eller inte – om det är en int eller en float eller
en bool kvittar.
För större allokeringar av data utan pekare behöver layoutspecikationen enbart
vara en __storleksangivelse__. I likhet med headern kan vi reservera en bit för
att ange om layoutspecikationen är en storlek i bytes, eller om det är en vektor
med mer precis layoutinformation. På en maskin där en pekare är 64 bitar skulle
alltså 2 bitar gå åt till metadata om headern, ytterligare 1 bit gå åt till att
koda in typ av layoutspecikation, och resterande 61 bitar antingen vara en
storlek i bytes eller en bitvektor och dess längd
