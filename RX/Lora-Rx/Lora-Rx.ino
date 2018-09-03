/*
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
*                                      RECIVER MODULE CODE
*     Board:Adafruit Feather 32u
*     Version:1.0
*     Author:Marco Manfrè
*     Date:30/08/2018
*     
*     Description:
*     "this code must be loaded on the receiving radio module: 
*     its task is to capture the data arriving from the remote stations and send them via serial 
*     to the ESP8266 module"
*     
*     Note:
*     "all the parts that you should edit
*     to make the code work as it is on your service are in the definitions part"
*     "The aux_struct is used to adapt the packet to serial transmission"
*     
*     Connections:
*     The LoRa module(Adafruit leather 32U ) is connected to the Node by 3 wires:
*       1.GND->GND
*       2.TX->D7
*       3 RX->D6
*     
*****************************************************************************************************
 
*/
#include <SoftwareSerial.h>
#include <RH_RF95.h>
#define RFM95_CS  8
#define RFM95_RST 4
#define RFM95_INT 7
#define RF95_FREQ 868.0 //define the working frequency of the module,all the module must be at the same frequency
#define WAIT 1000

int   Tpower =      23;//define the Tx power of the module
byte ECHO=1;

int   interruptPin =2;

RH_RF95 rf95(RFM95_CS, RFM95_INT);//create an istance of the Lora object

struct SensorPacket  {
//our struct recived by radio
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

struct ser_aux{
//this is an auxiliary struct for adapt the packet to be sent by serial
float ID;//1
float T0;//2
float T1;//3
float T2;//4
float Lux;//5
float Load;//6
float Temp_ext;//7
float Allarm;

  };
  
ser_aux floatdata;

SoftwareSerial mySerial(12,13);//rx,tx

void setup() {

 
 mySerial.begin(9600);
 Serial.begin(9600); 
                   

//setup the LoRa module
 pinMode(RFM95_RST, OUTPUT);           
 pinMode(interruptPin, INPUT);  
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

void show(){
  //a simple function that show the data for debugging purpose
  Serial.println(mydata.ID);
  Serial.println(mydata.T0);
  Serial.println(mydata.T1);
  Serial.println(mydata.T2);
  Serial.println(mydata.Lux);
  Serial.println(mydata.Load);
  Serial.println(mydata.Temp_ext);
  Serial.println(mydata.Allarm);
  }

void recv(){
  //fill the scruct and prepare the packet to be sent
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
  
void to_esp(){
  //Send the packet to  the esp 
  mySerial.write((byte*) &floatdata,sizeof(floatdata));
  }
  
void loop() {
  recv();
  to_esp();
  if(ECHO){
    show();
  }
  delay(WAIT);
}

