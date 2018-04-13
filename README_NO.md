# mbus-to-udp

## Innledning

Som alle andre her i landet fikk jeg en ny smart elmåler. Den heter Kamstrup måler tre faser og min netteier har sendt meg en HAN modul sammen med lykkeønskinger.

Modulen bruker, takket være NVEs visdom, en M-BUS grensesnitt. De kunne ha gått for RS-232 eller ethernet, men det hadde kanskje vært for enkelt.

Grensesnittet i modulen er blitt snudd og gjort om. Opprinnelig skulle måleren være en MBUS slave som svarte på forespørsler fra en master. I denne implementeringen er måleren en master som sender en strøm med data. Det vi trenger er en slave som tar denne strømmen imot og konverterer den til noe som kan brukes.

## Systembeskrivelse

HAN modulen er koblet ved hjelp av en standard nettverkskabel til en MBUS motteker som konverterer MBUS signal (24-36V) til UART (0-3,6V). Konverter sitter oppå en Raspberry Pi Zero W som tar imot seriedata, konverterer de og sender de ut som UDP. I min tilfelle sendes data til en Node-Red server som tar seg av visualiseringen og videre styring.

## Prosjektkomponenter

* **kicad** - Skjema over mottakerkretsen, kretskort og komponentliste
* **stl** - .stl filer for DIN montert boks
* **src** - kildekode til program som tar mot, dekoder og sender data.
* **nodered** - eksempelnode for visning av brukt effekt, strøm og spenning




 
   