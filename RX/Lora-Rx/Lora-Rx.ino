/*****************
 * *****************************************************
 * SMARTHIVE-STATION 
 * Microcontroller:ATMEGA32u
 * Trasmitter:RH_RF95
 * Author:Marco Manfrè
 * Version:1.0 alpha
 * Date:03/08/2018
 * ****************************************************
 *la Station si occupa di ricevere le strutture dal modulo LoRa
 *e inviare i dati al modulo esp8266(NodeMCU) via I2C
 *
 *colllegamenti:
 *(I2C)->GUARDA README
 */
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <I2C_Anything.h>
#include <RH_RF95.h>
#include "DHT.h" 
#include "HX711.h"

#define RFM95_CS  8
#define RFM95_RST 4
#define RFM95_INT 7
#define RF95_FREQ 868.0
#define WAIT 1000
int   Tpower =      23;
byte ECHO=1;
int   interruptPin =2;
RH_RF95 rf95(RFM95_CS, RFM95_INT);//creo l'istanza del modulo di trasmissione

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

struct i2cpayload{
//QUESTA STRUTTURA ESISTE ATTUALMENTE SOLO A FINI DI DEBUG SULLA TRASMISSIONE I2C
float ID;//1
float T0;//2
float T1;//3
float T2;//4
float Lux;//5
float Load;//6
float Temp_ext;//7
float Allarm;//8
  };
i2cpayload floatdata;

void setup() {
 //inizializzazione comunicazioni
 Wire.begin(8);                        //inizializzo la comunicazione con l'indirizzo di bus 8
 Wire.onRequest(requestEvent);         //al verificarsi della richiesta sul bus, esegue la funzione requestEvent
 Serial.begin(9600);                   //inizializzo la seriale del micro
 //****IMPOSTO I PIN DIGITALI****
 pinMode(RFM95_RST, OUTPUT);           
 pinMode(interruptPin, INPUT); 
 //resetto il modulo lora   
 digitalWrite(RFM95_RST, HIGH); 
 digitalWrite(interruptPin, LOW);     
 digitalWrite(RFM95_RST, LOW);
 delay(10);
 digitalWrite(RFM95_RST, HIGH);
 delay(10);
 
 while (!rf95.init()) {
    if (ECHO) {
      Serial.println("LoRa radio init failed");
    }
  }

  if (ECHO) {
    Serial.println("LoRa radio init OK!");
  }
rf95.setFrequency(RF95_FREQ);
   if (ECHO) {
    Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  }
  
  // imposto potenza di trasmissione
  rf95.setTxPower(Tpower, false);
    if (ECHO) {
    Serial.print("Set Power to: "); Serial.println(Tpower);
  }
 

}

void mostra(){
  //A fini di debug è utile mostrare i dati ricevuti via radio
  Serial.println(mydata.ID);
  Serial.println(mydata.T0);
  Serial.println(mydata.T1);
  Serial.println(mydata.T2);
  Serial.println(mydata.Lux);
  Serial.println(mydata.Load);
  Serial.println(mydata.Temp_ext);
  Serial.println(mydata.Allarm);
  }

void ricevi(){
  //immette all'interno del pacchetto i dati che riceve via radio
  lora_read(mydata);
  floatdata.ID=mydata.ID;
  floatdata.T0=mydata.T0;
  floatdata.T1=mydata.T1;
  floatdata.T2=mydata.T2;
  floatdata.Lux=mydata.Lux;
  floatdata.Load=mydata.Load;
  floatdata.Temp_ext=mydata.Temp_ext;
  floatdata.Allarm=mydata.Allarm;
  
  }
void loop() {
  ricevi();
  if(ECHO){
    mostra();
  }
  delay(WAIT);
}
void requestEvent() {
  /*
   * invia la struttura ricevuta via I2C al "master", il nostro modulo ESP
   */
  int i=0;
  
  Serial.println("sending data");
  
    I2C_writeAnything (floatdata);
  
  
}

