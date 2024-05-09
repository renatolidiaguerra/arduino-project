#include <WiFi.h>
#include <ESP32Ping.h> /*including the ping library*/

// #define pinLed  2

const char* ssid = "warbrito"; /*Define network SSID*/

const char* password = "gb240820"; /*Define Network Password*/

int pinMaster = 12;   /// trocar para releMaster e releSlave  +  pensar em led externo
int pinSlave = 27;

int pinLed = 2;

bool board_on = true;

int countHA = 0;
int countRouter = 0;

void setup() {

  Serial.begin(115200); /*Baud rate for serial communication*/
  pinMode(pinSlave, OUTPUT);
  pinMode(pinMaster, OUTPUT);

  pinMode(pinLed,OUTPUT);

  // for (int y = 0; y < 10; y++) {
  // digitalWrite(pinSlave,HIGH);   // low = on;  high=off
  // delay(1000);
  // digitalWrite(pinSlave,LOW);   // low = on;  high=off
  // delay(1000);
  // }

  digitalWrite(pinSlave,HIGH);   // low = on;  high=off
  digitalWrite(pinMaster,HIGH);   // low = on;  high=off

  digitalWrite(pinLed,HIGH);   

  digitalWrite(board_on, HIGH);

  Serial.println("\n\n\n\n\n\n\n\n\n\n");
  Serial.println("Starting system...");
}

void loop() {

  WiFi.begin(ssid, password); /*Begin WiFi connection*/

  while (WiFi.status() != WL_CONNECTED) {

    countRouter += 1;
    Serial.print("Connecting to WiFi... - attempt ");
    Serial.print(countRouter);
    Serial.println(" of 5");
    
   // delay de 1 minuto
    for (int i = 0; i < 120; i++) {
      delay(10);
      digitalWrite(pinLed,HIGH);
      delay(500);
      digitalWrite(pinLed,LOW);
      if (WiFi.status() == WL_CONNECTED) { break; }
    }

    if (countRouter >= 5) {     // fora durante 5 minutos
      Serial.println("Conection Router failed.");
      Serial.println("Shut down...");
      digitalWrite(pinLed,HIGH);
      digitalWrite(pinMaster,LOW);
      delay(1 * 60000);     // 1 minuto
      digitalWrite(pinMaster,HIGH);
      digitalWrite(pinLed,LOW);
      Serial.println("Starting Router...");
      delay(5 * 60000);     // 5 minuto
      countRouter = 0;
    }
  }

  Serial.println("Router online!");
  countRouter = 0;

  while (WiFi.status() == WL_CONNECTED) {

    digitalWrite(pinLed,HIGH);

    bool success = Ping.ping("192.168.15.10", 3);       // homeassistant.local:8123       Ping.ping("192.168.15.10", 3); 

    if (success) {
      Serial.println("Ping successful");
      countHA = 0;
    } else {

      countHA += 1;

      Serial.print("Ping failed... - attempt ");
      Serial.print(countHA);
      Serial.println(" of 5");
    }

    // delay de 1 minuto
    for (int i = 0; i < 6; i++) {
      if (countHA > 0) {
        for (int x = 0; x < 10; x++) {
          digitalWrite(pinLed,LOW);
          delay(1000);
          digitalWrite(pinLed,HIGH);
          delay(10);
        }
      } else {
        digitalWrite(pinLed,LOW);
        delay(10000);
        digitalWrite(pinLed,HIGH);
        delay(10);
      }
    }

    if (countHA >= 5) {     // fora durante 5 minutos
      Serial.println("Conection Home Assistant failed.");
      Serial.println("Shut down...");
      digitalWrite(pinLed,HIGH);
      digitalWrite(pinSlave,LOW);
      delay(1 * 60000);      // 1 minuyo
      digitalWrite(pinSlave,HIGH);
      Serial.println("Starting Home Assistant...");
      delay(5 * 60000);      // 5 minutos para restart do sistema
      Serial.println("Starting ping...");
      digitalWrite(pinLed,LOW);
      countHA = 0;
    }
  }
}