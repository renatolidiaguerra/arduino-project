// #include <WiFi.h>
#include "ESP8266WiFi.h"

// +----------------------------------+
// | CONFIGURAÇÕES PARA ESP8266 LOLin |
// |----------------------------------|
// | PLACA: LOLIN(WeMos) D1 R1        |
// | COM: qualquer uma                |
// +----------------------------------+
// pinger

// #include <ESP32Ping.h> /*including the ping library*/

const char* ssid = "warbrito"; /*Define network SSID*/
const char* password = "gb240820"; /*Define Network Password*/

//int pinGRN = 1;
//int pinYLW = 2;
int ledBoard = 2;

// int pinRele1 = 13;
// int pinRele2 = 14;

void setup() {

  Serial.begin(115200); /*Baud rate for serial communication*/
  // pinMode(pinGRN, OUTPUT);
  pinMode(ledBoard, OUTPUT);

  // Connect to WiFi
  WiFi.begin(ssid, password);

  // while wifi not connected yet, print '.'
  // then after it connected, get out of the loop
  while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
  }
  //print a new line, then print WiFi connected and the IP address
  Serial.println("");
  Serial.println("WiFi connected");
  // Print the IP address
  Serial.println(WiFi.localIP());

}

void loop() {

    digitalWrite(ledBoard,LOW);
    delay(250);
    digitalWrite(ledBoard,HIGH);
    delay(3000);


  // WiFi.begin(ssid, password); /*Begin WiFi connection*/

  // while (WiFi.status() != WL_CONNECTED) {
  //   digitalWrite(pinYLW,LOW);
  //   digitalWrite(pinGRN,HIGH);
  //   delay(250);
  //   digitalWrite(pinGRN,LOW);
  //   delay(250);

  //   Serial.println("Connecting to WiFi... - loop");
  // }

  // while (WiFi.status() == WL_CONNECTED) {

  //   bool success = Ping.ping("192.168.15.10", 3);       // homeassistant.local:8123       Ping.ping("192.168.15.10", 3); 

  //   if (!success) {
  //     Serial.println("Ping failed - loop");
  //     digitalWrite(pinGRN,LOW);
  //     digitalWrite(pinYLW,HIGH);
  //     delay(1000);
  //     digitalWrite(pinYLW,LOW);
  //     delay(1000);
  //     digitalWrite(pinYLW,HIGH);
  //     delay(1000);
  //     digitalWrite(pinYLW,LOW);
  //     delay(1000);
  //   } else {
  //     Serial.println("Ping successful. - loop");
  //     digitalWrite(pinGRN,HIGH);
  //     digitalWrite(pinYLW,LOW);
  //     delay(5000);
  //   }
  
  // }

}