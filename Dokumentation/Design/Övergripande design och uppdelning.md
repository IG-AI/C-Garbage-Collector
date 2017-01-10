Vi har valt att dela upp programmt i fyra delsystem: Stack search, Header, Alloc map och Heap.

Stack search är den del av programmet som ansvarar för att söka upp pekare i stacken.

Header ansvarar för att sätta och läsa metadata.

Alloc map är en "karta" av heapen som visar på vilka platser data är allokerat.

Heap är dels själva skräpsamlaren och dels den virituella heapen ovh dess allokeringsfunktioner.

Mer information om delsystemen finns i deras separata designdokument.
