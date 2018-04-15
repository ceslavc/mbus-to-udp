# mbus-to-udp

## Innledning

Som alle andre her i landet fikk jeg en smart elmåler. Den heter Kamstrup. Siden loven gir meg rett til å se mine data, har netteieren sendt meg en gratis HAN modul og ønsket med lykke til med å se dataene.

NVE har valgt en M-BUS grensesnitt. De kunne ha valgt RS-232 eller ethernet men det hadde kanskje vært for enkelt.

MBUS ble opprinnelig laget for å samle data fra mange målere i et stor område. Den tåler lange avstander og dårlige kabler. Meningen er at måleren skal være en slave som svarer på forespørsler fra en master innsamlingsenhet. Implementeringen av MBUS i HAN modulen derimot er helt motsatt  - måleren en master som sender en strøm med data. 

Det jeg trengte var en slave mottaker som kunne konvertere MBUS til en signal som kan brukes.

## Systembeskrivelse

Mottakerkretsen sitter oppå en Raspberry Pi Zero W. Kobling til HAN modulen er via en vanlig nettverkskabel. Datastrøm blir konvertert fra MBUS 24-36V nivå til UART spenningsnivå (3,6V) som leveres fra RPi. Mottakerkretsen er strømforsynt fra MBUS linje. Raspberry Pi gjør nødvendig dekoding og sender data ut som UDP over trådløs nett (derfor RPi W modellen). I min tilfelle sendes data til en Node-Red server som tar seg av visualisering og videre styring.

## Prosjektkomponenter

* **kicad** - Skjema over mottakerkretsen, kretskort og komponentliste
* **stl** - Boks for konverter som monteres på en DIN skinne
* **src** - kildekode til program som tar mot, dekoder og sender data.
* **nodered** - eksempelnode for visning av brukt effekt, strøm og spenning




 
   