/*
 * **************************************************
 * SMARTHIVE-ARNIA
 * Microcontroller:ATMEGA32u
 * Trasmitter:RH_RF95
 * Author:Marco Manfrè
 * Version:1.0 alpha
 * Date:03/08/2018
 * **************************************************
 * 
 * collegamenti:
 * sensori temperatura Dallas:DS18520->AO(pullup 4.7k)[ TO ;T1;T2]
 * sensore luminosità:A1
 * sensore cella di carico:(HX711)->A2,A3
 * sensore bolla:D8
 * sensore apertura/microswitch:D7
 * sensore ambiente esterno: DHT22->D9
 * #############
 * NOTA PER LA CELLA DI CARICO
 * avremo un offset calcolato sulla base dell'arnia vuota, che verrà introdotto a ogni reset
 * atto a sostiture la funzione set_tare() che porta tutto a 0
 * #############
 * 1.temperature interne(3)
 * 4.sensore luminosità
 * 5.cella di carico
 * 6.allarmi(microswitches)
 * 7.temperatura_ester
 * 8.ID
 * ################################
 * ID ALLARMI
 * 0001->Arnia aperta
 * 0010->Arnia rovesciata
 * 0100->
 * 1000->
 * ################################
 */
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <I2C_Anything.h>
#include <RH_RF95.h>
#include "DHT.h" 
#include "HX711.h"
//-------------definizioni-----------------
#define ONE_WIRE_BUS A0
#define LUX_SENS_PIN A1
#define LOADCELL_DAT A2
#define LOADCELL_CLK A3
#define BUBBLE_PIN   10
#define MICROSWITCH_PIN 11
#define DHT_PIN 9
#define DHT_TYPE DHT22
#define RFM95_CS  8
#define RFM95_RST 4
#define RFM95_INT 7
#define VBATPIN   A9  //  Pin where you can measure the battery power-OPTIONAL
#define RF95_FREQ 868.0
#define WAIT 10
#define UNIT_ID 1
int   Tpower =      23;
byte ECHO=1;
int   interruptPin =2;
RH_RF95 rf95(RFM95_CS, RFM95_INT);//creo l'istanza del modulo di trasmissione
HX711 scale(A2, A3);//creo l'istanza della cella di carico
DHT dht1(DHT_PIN, DHT_TYPE);
OneWire ourWire(ONE_WIRE_BUS);
DallasTemperature sensors(&ourWire);
//-----------------------------------------
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

void setup() {
 Wire.begin(8);
 Serial.begin(9600);
 sensors.begin();
 dht1.begin();
 pinMode(BUBBLE_PIN,INPUT);
 pinMode(MICROSWITCH_PIN,INPUT);
 //#####TRASMITTER INIT#######
 pinMode(RFM95_RST, OUTPUT);  // equivale al tasto del reset
 digitalWrite(RFM95_RST, HIGH);
 pinMode(interruptPin, INPUT);
 digitalWrite(interruptPin, LOW);
 // manual reset
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
  //############BILANCIA#################
  scale.set_scale(2280.f);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();// reset the scale to 0
  //°°°IMPOSTO ID UNITÀ°°°°°
  mydata.ID=UNIT_ID;
}
//##########FUNZIONI UTILI###############
void sendAllarm(){
  //COSTRUISCE E RIEMPIE IL CAMPO ALLARMI DELLA STRUTTURA
  int allarm=0;
  int stato_arnia;
  int stato_tappo;
    //controllo la posizione
    stato_arnia=digitalRead(BUBBLE_PIN);
    if(stato_arnia==1){
      allarm++;
    }
  //controllo tappo
  stato_tappo=digitalRead(MICROSWITCH_PIN);
   if(stato_tappo==1){
      allarm=allarm+10;
    }
    mydata.Allarm=allarm;
  }
  
void get_temp(){
  //RILEVA E RIEMPIE I CAMPI T[0 1 2] DELLA STRUTTURA
  sensors.requestTemperatures();
  mydata.T0=sensors.getTempCByIndex(0);
  mydata.T1=sensors.getTempCByIndex(1);
  mydata.T2=sensors.getTempCByIndex(2);
  
  }
void get_ext_temp(){
  //RILEVA E RIEMPIE I CAMPI Temp_ext DELLA STRUTTURA
  float t;
  t= dht1.readTemperature();
  mydata.Temp_ext=t;
}

void get_lux(){
  //RILEVA E RIEMPIE IL CAMPO LUX DELLA STUTTURA
  float sensorLux= analogRead(LUX_SENS_PIN);
  mydata.Lux=sensorLux;
}

void get_load(){
  //RILEVA E RIEMPIE IL CAMPO LOAD DELLA STRUTTURA
  float load;
  load=scale.get_units(10);
  mydata.Load=load;
}

void trasmetti(){
  //TRASMETTE VIA LORA LA STRUTTURA COSTRUITA
  lora_write(mydata);
  }
void mostra(){
  Serial.print("The lenght of my struct is:");
  Serial.println(sizeof(mydata));
  Serial.println(mydata.ID);
  Serial.println(mydata.T0);
  Serial.println(mydata.T1);
  Serial.println(mydata.T2);
  Serial.println(mydata.Lux);
  Serial.println(mydata.Load);
  Serial.println(mydata.Temp_ext);
  Serial.println(mydata.Allarm);
  }
//#######FINE FUNZIONI UTILI#######
void loop() {
  get_temp();
  get_ext_temp();
  get_lux();
  get_load();
  trasmetti();
  if(ECHO){
    mostra();
    }
  delay(WAIT);
  
}
