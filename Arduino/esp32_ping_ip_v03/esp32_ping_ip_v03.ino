#include <WiFi.h>

#include <ESP32Ping.h> /*including the ping library*/

#define ONBOARD_LED  2

const char* ssid = "warbrito"; /*Define network SSID*/

const char* password = "gb240820"; /*Define Network Password*/

int pinGRN = 12;
int pinYLW = 27;
bool board_on = true;

void setup() {

  Serial.begin(115200); /*Baud rate for serial communication*/
  pinMode(pinGRN, OUTPUT);
  pinMode(pinYLW, OUTPUT);

  pinMode(ONBOARD_LED,OUTPUT);

  digitalWrite(pinGRN,HIGH);
  digitalWrite(pinYLW,HIGH);
  digitalWrite(board_on, HIGH);
  // WiFi.begin(ssid, password); /*Begin WiFi connection*/

  // while (WiFi.status() != WL_CONNECTED) {

  //   delay(500);

  //   Serial.println("Connecting to WiFi...");
  // }

  // bool success = Ping.ping("192.168.15.10", 3); /*ping ESP32 using google*/

  // if (!success) {

  //   Serial.println("Ping failed");

  //   return;
  // }

  // Serial.println("Ping successful.");
}

void loop() {

  WiFi.begin(ssid, password); /*Begin WiFi connection*/

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(pinYLW,HIGH);
    digitalWrite(pinGRN,HIGH);
    delay(1000);

    Serial.println("Connecting to WiFi... - loop");
  }

  while (WiFi.status() == WL_CONNECTED) {
    digitalWrite(ONBOARD_LED,HIGH);
    bool success = Ping.ping("192.168.15.10", 3);       // homeassistant.local:8123       Ping.ping("192.168.15.10", 3); 
    //digitalWrite(ONBOARD_LED,LOW);

    if (success) {
      Serial.println("Ping successful");
      digitalWrite(pinGRN,LOW);
      digitalWrite(pinYLW,LOW);
    } else {
      Serial.println("Ping failed");
      digitalWrite(pinGRN,LOW);
      digitalWrite(pinYLW,HIGH);
    }

    for (int i = 0; i < 6; i++) {
      digitalWrite(ONBOARD_LED,LOW);
      delay(10000);
      digitalWrite(ONBOARD_LED,HIGH);
      delay(10);
    }
    // if (board_on) {
    //   board_on = false;
    //   digitalWrite(ONBOARD_LED,HIGH);
    //  // delay(200);
    // } else {
    //   board_on = true;
    //   digitalWrite(ONBOARD_LED,LOW);
    //  // delay(4800);
    // }
  //  delay(60000);
  }
}