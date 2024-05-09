#include <PubSubClient.h>
#include <WiFi.h>

// Informações do WiFi
#define WIFISSID "warbrito"
#define WIFIPASS "gb240820"

// Informações do Servidor MQTT
const char* mqttServer = "192.168.15.10";
const int mqttPort = 1883;
const char* mqttUser = "renatobrito";
const char* mqttPass = "gb240820";
const char* topicControl = "nodemcu/smartplug/control";
const char* topicState = "nodemcu/smartplug/state";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

int buttonPin = 5;
int ledBoard = 2;
bool lastButtonState = LOW;
bool currentButtonState = LOW;
bool guardState = LOW;

void setup() {
  Serial.begin(115200);

  pinMode(buttonPin, INPUT);
  pinMode(ledBoard, OUTPUT); // Definir pino do LED como saída
  
  WiFi.begin(WIFISSID, WIFIPASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  mqttClient.setServer(mqttServer, mqttPort);

  mqttClient.setCallback(callback);

  if (!mqttClient.connected()) {
    reconnect();
  }

  mqttClient.subscribe(topicControl); // Subscrever ao tópico para receber comandos

  //Configura Sensores através do MQTT Discovery do Home Assistant
  String buttonConfig = "{\"name\": \"SmartPlug\", \"command_topic\": \"nodemcu/smartplug/control\", \"state_topic\": \"nodemcu/smartplug/state\"}";
  mqttClient.publish("homeassistant/switch/nodemcu/smartplug/config", buttonConfig.c_str(), true);

}

void loop() {
  if (!mqttClient.connected()) {
    reconnect();
  }
  // Serial.print(".");
  mqttClient.loop(); // Manter a conexão MQTT ativa

  currentButtonState = digitalRead(buttonPin);
  if (currentButtonState == HIGH) {   // && lastButtonState == LOW) {
    guardState = !guardState;
    delay(250);
    Serial.println("Button Pressed");
    
    lastButtonState = currentButtonState;

    digitalWrite(ledBoard, guardState); // Ativar ou desativar o LED conforme o estado do botão

    if (guardState == LOW) {
      mqttClient.publish(topicState, "OFF");
    } else {
      mqttClient.publish(topicState, "ON");
    }
  }
  delay(100); // Pequeno atraso para evitar rebotes no botão
}

void reconnect() {
  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT...");
    if (mqttClient.connect("SmartPlug", mqttUser, mqttPass)) {
      Serial.println("Connected to MQTT");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Received message [");
  Serial.print(topic);
  Serial.print("]: ");

  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

  Serial.println();

  if (strcmp(topic, topicControl) == 0) {
    if (payload[0] == 'O' && payload[1] == 'N') {
      guardState = HIGH;
      digitalWrite(ledBoard, HIGH);
      mqttClient.publish(topicState, "ON");
    } else if (payload[0] == 'O' && payload[1] == 'F' && payload[2] == 'F') {
      guardState = LOW;
      digitalWrite(ledBoard, LOW);
      mqttClient.publish(topicState, "OFF");
    }
  }
}
