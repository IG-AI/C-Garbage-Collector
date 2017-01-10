
* Programmet fungerar inte bra på SPARC. Några tester misslyckas i SPARC och vi har försökt fixa det, utan framgång. Det som blir fel är att på SPARC hittas det pekare på stacken in i heapen som inte borde hittas. Vi tror att detta beror på skillnaden i hur stacken växer i SPARC. Det skulle ha löst sig om vi kunde använda `__builtin_frame_address()` men då vi behövde byta till _Suns C Compiler_ för SPARC gick inte denna lösning. Det går att köra programmet i SPARC, men vi misstänker, på grund av testerna som misslyckas, att skräpsamlingen inte kommer att genomföras på ett korrekt sätt. Sen så fungerar inte valgrind heller i SPARC så det är väldigt svårt att testa om skräpsamlingen sker korrekt eller inte. För att fixa detta problem om vi hade haft mer tid skulle vi ha hittat ett sätt att få fram stack-frame addressen liknande `__builtin_frame_address()` fast för _Suns C Compiler_

* Vi dumpar inte registren på stacken. Detta på grund av att när vi implementerar funktionen som dumpar registren så fungerar inte våran kod. Dessutom så fungerar vårat program till synes bra utan den funktionen.

* h_used räknar nu även med padding och headers. Detta eftersom vi anser att det är mera logiskt att ha med padding och headers i uträkningen.

* Allokeringskartan är en bytemap snarare än en bitmap. Men den fungerar som den ska, enda nackdelen är att den är att den tar större plats i minnet vilket inte känns som ett stort problem. 

* Vi har valt att inte implementera höga adresser. Detta eftersom det inte var ett krav och för att vi aldrig fick den att fungera. Vi gjorde försök med posix_memalign som ska kunna användas för att be om högre adresser men vi lyckades bara få den att ge hos marginellt större adresser ibland och ibland fick vi även mindre adresser.

* Vi kan inte få ut någon code coverage för vissa moduler. Vi har försökt fixa detta väldigt länge utan framgång. Mer information finns i dokumentet för enhetstestning. 

* Vi fick aldrig BDW GC att fungera så våra performance tests saknar jämförelse med denna. Punkt 2 av Specens Performance jämförelser blev inte heller klar då programmet gav Segfault när data fylldes andra gången.
