/*
 *
******************************************************************************************************
 ________  _____ ______   ________  ________  _________  ___  ___  ___  ___      ___ _______         *
|\   ____\|\   _ \  _   \|\   __  \|\   __  \|\___   ___\\  \|\  \|\  \|\  \    /  /|\  ___ \        *
\ \  \___|\ \  \\\__\ \  \ \  \|\  \ \  \|\  \|___ \  \_\ \  \\\  \ \  \ \  \  /  / | \   __/|       *
 \ \_____  \ \  \\|__| \  \ \   __  \ \   _  _\   \ \  \ \ \   __  \ \  \ \  \/  / / \ \  \_|/__     *
  \|____|\  \ \  \    \ \  \ \  \ \  \ \  \\  \|   \ \  \ \ \  \ \  \ \  \ \    / /   \ \  \_|\ \    *
    ____\_\  \ \__\    \ \__\ \__\ \__\ \__\\ _\    \ \__\ \ \__\ \__\ \__\ \__/ /     \ \_______\   *
   |\_________\|__|     \|__|\|__|\|__|\|__|\|__|    \|__|  \|__|\|__|\|__|\|__|/       \|_______|   *
   \|_________|                                                                                      *
****************************************************************************************************** 
*                                       TRANSMITTER MODULE CODE
*     
*     Microcontroller:ATMEGA32u
*     Trasmitter:RH_RF95
*     Author:Marco Manfrè
*     Version:1.0 alpha
*     Date:03/08/2018
* 
*     Description:
*     
*     Connections:
*     Temperature sensor Dallas:DS18520->AO(pullup 4.7k)[ TO ;T1;T2]
*     Luminosity sensor:A1
*     Load Cell:(HX711)->A2,A3
*     microswitch:D7
*     environmental sensor: DHT22->D9
* 
* 
*     Note:
*     "we will have an offset calculated on the basis of the empty apiary, which will be 
*     introduced at each reset to replenish the set_tare () function that brings everything to 0"
*     "all the parts that you should edit
*     to make the code work as it is on your service are in the definitions part"
*     
* 
*     Allarm codes:
*     0001->Hive open
*     0010->Hive inverted
*     0100->free
*     1000->free
*******************************************************************************************************     
*     
 */
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <I2C_Anything.h>
#include <RH_RF95.h>
#include "DHT.h" 
#include "HX711.h"

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
#define RF95_FREQ 868.0
#define WAIT 10
#define UNIT_ID 1

int   Tpower =      23;

byte ECHO=1;

int   interruptPin =2;
RH_RF95 rf95(RFM95_CS, RFM95_INT);//create an istance of the Lora object

HX711 scale(A2, A3);//create an istance of the Load cell object

DHT dht1(DHT_PIN, DHT_TYPE);
OneWire ourWire(ONE_WIRE_BUS);
DallasTemperature sensors(&ourWire);


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
 
 //setup the LoRa module
 pinMode(RFM95_RST, OUTPUT);  // equivale al tasto del reset
 digitalWrite(RFM95_RST, HIGH);
 pinMode(interruptPin, INPUT);
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
  
  //setting up the TX power
  rf95.setTxPower(Tpower, false);
    if (ECHO) {
    Serial.print("Set Power to: "); Serial.println(Tpower);
  }


  
  //setting up the scale
  scale.set_scale(2280.f);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();// reset the scale to 0
  mydata.ID=UNIT_ID;
}

//utility functions
void sendAllarm(){
  //fill the allarms field
  int allarm=0;
  int stato_arnia;
  int stato_tappo;
    //check hive position
    stato_arnia=digitalRead(BUBBLE_PIN);
    if(stato_arnia==1){
      allarm++;
    }
  //check if the top is lifted
  stato_tappo=digitalRead(MICROSWITCH_PIN);
   if(stato_tappo==1){
      allarm=allarm+10;
    }
    mydata.Allarm=allarm;
  }
  
void get_temp(){
  //fill the internal temperature fields
  sensors.requestTemperatures();
  mydata.T0=sensors.getTempCByIndex(0);
  mydata.T1=sensors.getTempCByIndex(1);
  mydata.T2=sensors.getTempCByIndex(2);
  
  }
void get_ext_temp(){
  //fill the external temperature fields
  float t;
  t= 12.00;
  mydata.Temp_ext=t;
}

void get_lux(){
  //fill the luminosity field
  float sensorLux= analogRead(LUX_SENS_PIN);
  mydata.Lux=sensorLux;
}

void get_load(){
 //fill the load cell field
  float load;
  load=scale.get_units(10);
  mydata.Load=load;
}

void broadcast(){
  //Send the data over the air
  
  lora_write(mydata);
  
  }

void show(){
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

void loop() {
  
  get_temp();
  get_ext_temp();
  get_lux();
  get_load();
  broadcast();
  if(ECHO){
    show();
    }
    
  delay(WAIT);
  
}
