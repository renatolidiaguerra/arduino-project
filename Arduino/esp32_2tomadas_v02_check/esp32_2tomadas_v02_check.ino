#include <WiFi.h>

#include <ESP32Ping.h> /*including the ping library*/

#define ONBOARD_LED  2

const char* ssid = "warbrito"; /*Define network SSID*/

const char* password = "gb240820"; /*Define Network Password*/

int pinRele2 = 12;
int pinRele1 = 27;
bool board_on = true;

int countHA = 0;
int countRouter = 0;

void setup() {

  Serial.begin(115200); /*Baud rate for serial communication*/
  pinMode(pinRele1, OUTPUT);
  pinMode(pinRele2, OUTPUT);

  pinMode(ONBOARD_LED,OUTPUT);

  digitalWrite(pinRele1,LOW);   // low = on;  high=off
  digitalWrite(pinRele2,LOW);   // low = on;  high=off

  digitalWrite(board_on, HIGH);

  Serial.println("\n\n\n\n\n\n\n\n\n\n");
  Serial.println("Starting system...");
}

void loop() {

  WiFi.begin(ssid, password); /*Begin WiFi connection*/

  while (WiFi.status() != WL_CONNECTED) {

   // delay de 1 minuto
    for (int i = 0; i < 120; i++) {
      digitalWrite(ONBOARD_LED,LOW);
      delay(500);
      digitalWrite(ONBOARD_LED,HIGH);
      delay(10);
    }
    countRouter += 1;
    Serial.print("Connecting to WiFi... - attempt ");
    Serial.print(countRouter);
    Serial.println(" of 5");

    if (countRouter >= 5) {     // fora durante 5 minutos
      Serial.println("Conection Router failed.");
      Serial.println("Shut down...");
      digitalWrite(ONBOARD_LED,HIGH);
      digitalWrite(pinRele2,HIGH);
      delay(60000);     // 1 minuto
      digitalWrite(pinRele2,LOW);
      digitalWrite(ONBOARD_LED,LOW);
      Serial.println("Starting Router...");
      countRouter = 0;
    }
  }

  Serial.println("Router online!");

  while (WiFi.status() == WL_CONNECTED) {
    digitalWrite(ONBOARD_LED,HIGH);
    bool success = Ping.ping("192.168.15.10", 3);       // homeassistant.local:8123       Ping.ping("192.168.15.10", 3); 

    if (success) {
      Serial.println("Ping successful");
    } else {

      countHA += 1;

      Serial.print("Ping failed... - attempt ");
      Serial.print(countHA);
      Serial.println(" of 5");
    }

    // delay de 1 minuto
    for (int i = 0; i < 60; i++) {
      digitalWrite(ONBOARD_LED,LOW);
      delay(10000);
      digitalWrite(ONBOARD_LED,HIGH);
      delay(10);
    }

    if (countHA >= 5) {     // fora durante 5 minutos
      Serial.println("Conection Home Assistant failed.");
      Serial.println("Shut down...");
      digitalWrite(ONBOARD_LED,HIGH);
      digitalWrite(pinRele1,HIGH);
      delay(60000);      // 1 minuyo
      digitalWrite(pinRele1,LOW);
      digitalWrite(ONBOARD_LED,LOW);
      Serial.println("Starting Home Assistant...");
      countHA = 0;
    }
  }
}