# PDS projekt - Zprovozneni automatizace automatu na vydavani kavy

Cieľom tohto projektu je s pomocou MQTT protokolu zprevádzkovať automatizáciu automatu na vydávanie kávy. Projek bol zadaný a vznikol v rámci Fakulty Informačních Technologií VUT v Brně, ako študentsk projekt k predmetu PDS.

## Popis projektu

Pri implementácií projektu bola využitá platforma ESP8266, ktorá poskytuje pripojenie k bezdrôtovej WiFi sieti. ESP8266 umožňuje na diaľku ovládať činnosť kávovaru a taktiež monitorovať jeho aktuálny stav.

ESP8266 funguje ako MQTT klient a prostrednístvom WiFi komunikuje s MQTT serverom. Klient odosiela pomocou metódy publish údaje o kávovare, načítanej čipovej karte a potvrdzujúce správy prijímaných príkazov. Prostredníctvom metódy subscribe prijíma riadiace príkazy od aplikácie. Pomocou jednotlivých riadiacich príkazov je možné ovládať kávovar.  
