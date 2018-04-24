# PDS projekt - Zprovozneni automatizace automatu na vydavani kavy

Cieľom tohto projektu je s pomocou MQTT protokolu zprevádzkovať automatizáciu automatu na vydávanie kávy. Projek bol zadaný a vznikol v rámci Fakulty Informačních Technologií VUT v Brně, ako študentsk projekt k predmetu PDS.

## Popis projektu

Pri implementácií projektu bola využitá platforma ESP8266, ktorá poskytuje pripojenie k bezdrôtovej WiFi sieti. ESP8266 umožňuje na diaľku ovládať činnosť kávovaru a taktiež monitorovať jeho aktuálny stav.

ESP8266 funguje ako MQTT klient a prostrednístvom WiFi komunikuje s MQTT serverom. Klient odosiela pomocou metódy publish údaje o kávovare, načítanej čipovej karte a potvrdzujúce správy prijímaných príkazov. Prostredníctvom metódy subscribe prijíma riadiace príkazy od aplikácie. Pomocou jednotlivých riadiacich príkazov je možné ovládať kávovar.  

## Zapojenie

![alt text](https://github.com/hajdiktomas/PDS-projekt/blob/master/img/Schema_zapojenia.jpg)

## Použité knižnice
..* SPI (komunikácia s čítačkou kariet) dostupná: https://github.com/arduino/Arduino/tree/master/hardware/arduino/avr/libraries/SPI.
..* MQTT klient (PubSubClient.h) dostupná:  https://github.com/knolleary/pubsubclient.
..* Spracovanie JSON súborov (ArduinoJson.h) dostupná: https://github.com/bblanchon/ArduinoJson.
..* Bezdrôtový OTA update (ArduinoOTA.h) dostupná: https://github.com/esp8266/Arduino/tree/master/libraries/ArduinoOTA.
..* Čítačka čipových kariet (MFRC522.h) dostupná: https://github.com/miguelbalboa/rfid.

## Komunikačný MQTT protokol

Po pripojení ESP8266 ku MQTT sa ESP snaží udržiavať pripojenie stále aktívne. V prípade výpadku spojenia s WiFi alebo MQTT ESP8266 využíva nastavený parameter retained, ktorý zaistí opätovné zaslanie správy. Identifikátor zariadenia je "ESP8266Client".

### Subscribe

Tieto správy slúžia na prijímanie príkazov od MQTT Brokeru. Správy sú posielané s nastaveným parametrom Topic.

#####Topic:
..* coffee/cmd

#####Správy:
..* {"Clean":1}
..* {"TurnOn":1}
..* {"TurnOff":1}
..* {"MakeCoffe":1}


### Publish
