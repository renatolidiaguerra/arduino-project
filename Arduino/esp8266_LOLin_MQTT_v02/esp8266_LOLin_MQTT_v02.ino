#include <PubSubClient.h>
#include <ESP8266WiFi.h>

// Informações do WiFi
#define WIFISSID "warbrito"
#define WIFIPASS "gb240820"

// Informações do Servidor MQTT
const char* mqttServer = "192.168.15.10";
const int mqttPort = 1883;
const char* mqttUser = "renatobrito";
const char* mqttPass = "gb240820";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

int buttonPin = D6;
bool lastButtonState = LOW;
bool currentButtonState = LOW;
bool guardState = LOW;

void setup() {
  Serial.begin(115200);

  pinMode(buttonPin, INPUT);
  pinMode(D7, OUTPUT);
  
  WiFi.begin(WIFISSID, WIFIPASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  mqttClient.setServer(mqttServer, mqttPort);

  if (!mqttClient.connected()) {
    reconnect();
  }
}

void loop() {
  if (!mqttClient.connected()) {
    reconnect();
  }
  
  currentButtonState = digitalRead(buttonPin);
  if (currentButtonState == HIGH && lastButtonState == LOW) {
    guardState = !guardState;
    delay(250);
    Serial.println("Button Pressed");
  }
  lastButtonState = currentButtonState;
  
  digitalWrite(D7, guardState); // Ativar LED para indicar o estado do botão

  if (guardState == LOW) {
    mqttClient.publish("esp8266/botao2/state", "OFF");
  } else {
    mqttClient.publish("esp8266/botao2/state", "ON");
  }

  delay(100); // Pequeno atraso para evitar rebotes no botão
}

void reconnect() {
  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT...");
    if (mqttClient.connect("esp8266", mqttUser, mqttPass)) {
      Serial.println("Connected to MQTT");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}
