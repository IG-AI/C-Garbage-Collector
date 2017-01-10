
Programmet fungerar inte bra på SPARC. Några tester misslyckas i SPARC och vi har försökt fixa det, utan framgång. Det går att köra programmet i SPARC, men vi misstänker, på grund av testerna som misslyckas, att skräpsamlingen inte kommer att genomföras på ett korrekt sätt. Sen så fungerar inte valgrind heller i SPARC så det är väldigt svårt att testa om skräpsamlingen sker korrekt eller inte.

Vi dumpar inte registren på stacken. Detta på grund av att när vi implementerar funktionen som dumpar registren så fungerar inte våran kod. Dessutom så fungerar vårat program alldels utmärkt utan den funktionen.

h_used räknar nu även med padding och headers. Detta eftersom vi anser att det är mera logiskt att ha med padding och headers i uträkningen.

Allokeringskartan är en bytemap snarare än en bitmap. 

Vi har valt att inte implementera höga adresser. Eftersom det inte var ett krav och för att vi aldrig fick den att fungera. Vi gjorde försök med posix_memalign som ska kunna användas för att be om högre adresser. Men vi lyckades bara få den att ge hos marginellt större adresser ibland och ibland fick vi även mindre adresser.
