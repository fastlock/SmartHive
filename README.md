[logo]:logo.png
![logo]
## A OpenSource Hive monitoring system
SmartHive nasce da un idea semplice di un apicoltore hobbista: monitorare via internet le attività e lo stato di salute delle arnie.

E' caratterizzato dalla possibilità di controllare colonie molto distanti tramite l'utilizzo di radio **LoRa** e dal suo basso costo di realizzazione. 
## Il sistema
SmartHive si compone di:
* Sensori(installati direttamente sull'arnia)
* Beacon trasmittente
* Station ricevente

Il sistema è alimentato da un pannello fotovoltaico installato nei pressi della colonia
### I sensori
I sensori più importanti per monitorare l'attività dell'alveare sono:
* Temperatura interna (DS18520)
* Luminosità(photoresistor)
* Umidità relativa esterna(DHT22)
* Peso complessivo(celle di carico da bilancia pesapersone + HX711)

I sensori verranno collegati alla centralina anttraverso dei cavi all'esterno all'arnia.
La scelta dei componenti utilizzati nel progetto è mirata alla replicabilità dello stesso,il funzionamento di tutti i sensori è ampiamente documentato sul web. 
### Beacon trasmittente
La centralina dell'arnia è costituita principalmente da un microcontrollore ATMEGA32u a cui sono collegati i sensori, da un modulo LoRa e dalla sua antenna.

il suo compito è quello di inviare,a intervalli regolari, un pacchetto contenente la telemetria alla stazione centrale. 
### Station ricevente
La Stazione ricevente è una gemella del beacon(il modulo radio è impostato come ricevente),a cui è collegato un modulo ESP8266 per inviare il pacchetti ricevuti al cloud (thingspeak) grazie alla connessione WiFi.
