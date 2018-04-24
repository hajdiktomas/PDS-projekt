# PDS projekt - Zprovozneni automatizace automatu na vydavani kavy

Cieľom tohto projektu je s pomocou MQTT protokolu zprevádzkovať automatizáciu automatu na vydávanie kávy. Projek bol zadaný a vznikol v rámci Fakulty Informačních Technologií VUT v Brně, ako študentsk projekt k predmetu PDS.

## Popis projektu

Pri implementácií projektu bola využitá platforma ESP8266, ktorá poskytuje pripojenie k bezdrôtovej WiFi sieti. ESP8266 umožňuje na diaľku ovládať činnosť kávovaru a taktiež monitorovať jeho aktuálny stav.

ESP8266 funguje ako MQTT klient a prostrednístvom WiFi komunikuje s MQTT serverom. Klient odosiela pomocou metódy publish údaje o kávovare, načítanej čipovej karte a potvrdzujúce správy prijímaných príkazov. Prostredníctvom metódy subscribe prijíma riadiace príkazy od aplikácie. Pomocou jednotlivých riadiacich príkazov je možné ovládať kávovar.  

## Zapojenie

![alt text](https://github.com/hajdiktomas/PDS-projekt/blob/master/img/Schema_zapojenia.jpg)

## Použité súčiastky

- ESP8266 -12E Development Board
- RFID-RC522 13.56MHz modul
- LED dióda (indikácia konektivity)
- 300ohm rezistor (ochrana LED diódy)
- 8-kanálový reléový modul DC 5V s optočlenom
- Kávovar

## Použité knižnice
- SPI (komunikácia s čítačkou kariet) dostupná: https://github.com/arduino/Arduino/tree/master/hardware/arduino/avr/libraries/SPI.
- MQTT klient (PubSubClient.h) dostupná:  https://github.com/knolleary/pubsubclient.
- Spracovanie JSON súborov (ArduinoJson.h) dostupná: https://github.com/bblanchon/ArduinoJson.
- Bezdrôtový OTA update (ArduinoOTA.h) dostupná: https://github.com/esp8266/Arduino/tree/master/libraries/ArduinoOTA.
- Čítačka čipových kariet (MFRC522.h) dostupná: https://github.com/miguelbalboa/rfid.

## Komunikačný MQTT protokol

Po pripojení ESP8266 ku MQTT sa ESP snaží udržiavať pripojenie stále aktívne. V prípade výpadku spojenia s WiFi alebo MQTT ESP8266 využíva nastavený parameter retained, ktorý zaistí opätovné zaslanie správy. Identifikátor zariadenia je "ESP8266Client".

### Subscribe

Tieto správy slúžia na prijímanie príkazov od MQTT Brokeru. Správy sú posielané s nastaveným parametrom Topic.

Topic:

- coffee/cmd

Správy:

- {"Clean":1}
- {"TurnOn":1}
- {"TurnOff":1}
- {"MakeCoffe":1}


### Publish

Tieto správy slúžiia na odosielanie ID aktuálne načítanej čipovej karty, potvrdzovanie príkazov odoslaných z MQTT Brokeru a odosielanie aktuálneho stavu zariadenia. Odosielanie prebieha s nastaveným parametrom Topic. Pri načítaní čipovej karty sa odošle správa obsahujúca ID načítanej karty. Po prijatí príkazu zo strany MQTT Brokeru sa odošle potvrdzujúca správa o vykonaní alebo odmietnutí prijatého príkazu. Kedykoľvek sa zmení stav kávovaru napríklad tým, že nie je v zásobníku dostatočné množstvo vody odošle sa správa o stave kávovaru.

Topic:

- coffee/stat

Správa o načítaní čipovej karty:

- {"Action":"ReadCard","CardID":<uint32>}

Správa o potvrdení vykonania príkazu:

- {"Action":"Cleaning","State":<"Of"/"Off">}
- {"Action":"TurningOn","State":<"Of"/"Off">}
- {"Action":"TurningOff","State":<"Of"/"Off">}
- {"Action":"MakingCoffee","State":<"Of"/"Off">}

Správa o zmene stavu zariadenia:

- {"EmptyWatter":<"true"/"false">,"Ready":<"true"/"false">}
