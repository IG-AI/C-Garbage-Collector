
Programmet fungerar inte på SPARC

Vi dumpar inte registren på stacken. Detta på grund av att när vi implementerar funktionen som dumpar registren så fungerar inte våran kod. Dessutom så fungerar vårat program alldels utmärkt utan den funktionen.

h_used räknar nu även med padding och headers. Detta eftersom vi anser att det är mera logiskt att ha med padding och headers i uträkningen.

Allokeringskartan är en bytemap snarare än en bitmap. 

Vi har valt att inte implementera höga adresser. Eftersom det inte var ett kvar och för att vi aldrig fick den att fungera. Vi gjorde försök med posix_memalign som ska kunna användas för att be om högre adresser. Men vi lyckades bara få den att ge hos marginellt större adresser ibland och ibland fick vi även mindre adresser.
