/*
 * SMARTHIVE-MODULO ESP 
 * Si occupa di ricevere via i2c i pacchetti dalla station LoRa e inviarli via wifi a thingspeak
   ---------------------------------------------------
   CONNESSIONI:
   NODE MCU->ARDUINO PRO MICRO
   SDA:D1->A4(Analog pin)
   SCL:D2->A5(Analog pin)
   GND->GND(il ground DEVE essere condiviso)
*/

#include <I2C_Anything.h>
#include<Wire.h>
#include <ESP8266WiFi.h>
#define SlaveAddress 8            //definisco indirizzo periferica
#define MY_SSID "XXXX"            //SSID dell'hotspot
#define MY_PASSWORD "XXXX"        //password del hotspot
#define CHANNEL_ID 12345          //ID del canale
#define SECRET_API_KEY "xxxxxxx"  //chiave segreta per i permessi di scrittura sul canale
#define WAIT 1000                 //ms di attesa tra l'invio di una richiesta e l'altra
struct SensorPacket  {
word ID;//1
float T0;//2
float T1;//3
float T2;//4
float Lux;//5
float Load;//6
float Temp_ext;//7
float Allarm;//8
  };
SensorPacket mydata;

// ThingSpeak Settins

const char*  ssid=MY_SSID;
const char*  password=MY_PASSWORD;
const int channelID = CHANNEL_ID;
String writeAPIKey = SECRET_API_KEY; // write API key for your ThingSpeak Channel
byte ECHO=1;//per mostrare i passaggi a fini di debug
const char* server = "api.thingspeak.com";
const int postingInterval = 1000;//20 * 1000; // post data every 20 seconds
WiFiClient client;
void setup() {
  Wire.begin(2, 1); // inizializzo i pin sda, scl per la comunicazione
  Serial.begin(9600);  // inizializzo la comunicazione seriale solo per DEBUG
  Wire.begin(0,4);
  WiFi.begin(ssid, password);
  Serial.begin(9600); // start serial for out
   while (WiFi.status() != WL_CONNECTED) {
    delay(500);
}
}

/* Questa è la struttura dati da inviare
  qui si inserisce il prototipo del pacchetto che riceverà il master
  ##IMPORTANTE##
  MASTER E SLAVE DEVONO AVERE LO STESSO PROTOTIPO
*/
void sendToCloud() {
  //Questa funzione si occupa di costruire la richiesta HTTP da inviare ai server di Thingspeak
    if (client.connect(server, 80)) {
  

    String body = "field1=";
           body += String(mydata.T0);
           body += "&";
           body += "field2=";
           body += String(mydata.T1);
           body += "&";
           body += "field3=";
           body += String(mydata.T2);
           body += "&";
           body += "field4=";
           body += String(mydata.Lux);
           body += "&";
           body += "field5=";
           body += String(mydata.Load);
           body += "&";
           body += "field6=";
           body += String(mydata.Temp_ext);
           body += "&";
           body += "field8=";
           body += String(mydata.Allarm);
    

    client.println("POST /update HTTP/1.1");
    client.println("Host: api.thingspeak.com");
    client.println("User-Agent: ESP8266 (nothans)/1.0");
    client.println("Connection: close");
    client.println("X-THINGSPEAKAPIKEY: " + writeAPIKey);
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: " + String(body.length()));
    client.println("");
    client.print(body);

  }
  client.stop();

  
  

}

void mostra(){
  Serial.println(mydata.ID);
  Serial.println(mydata.T0);
  Serial.println(mydata.T1);
  Serial.println(mydata.T2);
  Serial.println(mydata.Lux);
  Serial.println(mydata.Load);
  Serial.println(mydata.Temp_ext);
  Serial.println(mydata.Allarm);
  }

void loop() {



  Wire.requestFrom(SlaveAddress, sizeof(SensorPacket ));// Richiedo struttura allo slave (attenzione a come è fatta la funzione:Wire.requestFrom(indirizzo slave,byte da ricevere)
  delay(100);
  I2C_readAnything (mydata);//quello che leggo riempie la mia struttura "vuota"
  
  if(ECHO){
    Serial.println("...........");
    mostra();
    Serial.println("...........");
  }
 
  sendToCloud ();
  
  delay(WAIT);



}

