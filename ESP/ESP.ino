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
*                                         ESP8266 CODE
*     Board:NodeMCU 1.0
*     Version:1.0
*     Author:Marco Manfrè
*     Date:30/08/2018
*     
*     Description:
*     "This code is listening passively on the serial (SoftwareSerial), 
*     it makes a cast of the received stream inside the struct and finally 
*     uses the Wlan connection to send a http write request on the Thingspeak server."
*     
*     Note:
*     "all the parts that you should modify 
*     to make the code work as it is on your service are in the definitions part"
*     
*     Connections:
*     The LoRa module(sparkfun leather 32U ) is connected to the Node by 3 wires:
*       1.GND->GND
*       2.TX->D7
*       3 RX->D6
*     
*****************************************************************************************************
 
*/



#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#define TX_PIN D7                                        //Serial TX pin
#define RX_PIN D6                                       //Serial RX pin

#define CHANNEL_ID 565995                             //check on "channel details" the id of your thingspeak channel

#define WAIT 1000                                  //this define the latency between one request to another (ms)


struct SensorPacket  {
/*
 * This struct contains the data of our sensors 
 */
float ID;//1         ID of the Hive
float T0;//2         Temperature
float T1;//3
float T2;//4
float Lux;//5        Luminosity
float Load;//6       Weight from the scale
float Temp_ext;//7   Outside Temperature
float Allarm;//8     Allarm Code
  };
SensorPacket mydata;

SoftwareSerial myserial(TX_PIN,RX_PIN);

const char*  ssid=    "ssid"  ;
const char*  password="passsword" ;
const int channelID = CHANNEL_ID  ;
String writeAPIKey=  "apikey"   ; 
const char* server = "api.thingspeak.com";


WiFiClient client;

byte ECHO=1;//for debug purpose,when is set,the user serial interface is on 

void setup() {

  myserial.begin(9600);
  if(ECHO)
    Serial.begin(9600);  
  WiFi.begin(ssid, password);
 Serial.println("connected");

}


void sendToCloud() {
  //this function build and send the message to the server 
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
  else{
    if(ECHO)
      Serial.println("**Connection error**");
  }
  client.stop();

  
  

}


void show(){
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

if(myserial.available()){
  recv();
  if(ECHO){
      Serial.println("...........");
      show();
      Serial.println("...........");
    }
  sendToCloud ();
  delay(WAIT);
  }
 
}

void recv(){
  //this funtion takes the raw data from the serial and build our struct
    uint8_t * p = (uint8_t*) &mydata;
    unsigned int i;
    for (i = 0; i < sizeof mydata; i++){
       
              *p=myserial.read();
              p=p+1;
              delay(5);
         }
   
}
//\0-0/

