
// #include <WiFi.h>
// #include <ESP32Ping.h> /*including the ping library*/

const char* ssid = "warbrito"; /*Define network SSID*/
const char* password = "gb240820"; /*Define Network Password*/

//int pinGRN = 1;
int pinYLW = D6;
int ledBoard = 2;

// int pinRele1 = 13;
// int pinRele2 = 14;

void setup() {

  Serial.begin(115200); /*Baud rate for serial communication*/
  // pinMode(pinGRN, OUTPUT);
  pinMode(pinYLW, OUTPUT);
  pinMode(ledBoard, OUTPUT);

  // pinMode(pinRele1, OUTPUT);
  // pinMode(pinRele2, OUTPUT);

  // digitalWrite(pinGRN,LOW);
  digitalWrite(pinYLW,LOW);

  // WiFi.begin(ssid, password); /*Begin WiFi connection*/

  // digitalWrite(pinRele1,LOW);
  // delay(1000);
  // digitalWrite(pinRele2,LOW);
  // delay(1000);
  // digitalWrite(pinRele1,HIGH);
  // delay(1000);
  // digitalWrite(pinRele2,HIGH);
  // delay(1000);
  // digitalWrite(pinRele1,LOW);
  // delay(1000);
  // digitalWrite(pinRele2,LOW);
  // delay(1000);
  // digitalWrite(pinRele1,HIGH);
  // delay(1000);
  // digitalWrite(pinRele2,HIGH);
  // delay(1000);

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

    digitalWrite(ledBoard,HIGH);
    delay(250);
    digitalWrite(ledBoard,LOW);
    delay(250);

    digitalWrite(pinYLW,HIGH);
    delay(250);
    digitalWrite(pinYLW,LOW);
    delay(250);

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