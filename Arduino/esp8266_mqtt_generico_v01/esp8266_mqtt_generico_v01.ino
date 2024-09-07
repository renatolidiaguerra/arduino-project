#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#include <ArduinoJson.h>

#define DHTPIN D5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const char *WIFI_SSID = "warbrito";
const char *WIFI_PASSWORD = "gb240820";

const char* mqttServer = "192.168.15.10";  // O IP do seu broker MQTT
const int mqttPort = 1883;
const char* mqttUser = "renatobrito";
const char* mqttPassword = "gb240820";

int sensorPin = A0;  // Pino do sensor analógico

int sensorNumber = 11;
String mqttName = "Generic sensor " + String(sensorNumber);
String stateTopic = "home/generic/" + String(sensorNumber) + "/state";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void setup() {
  Serial.begin(9600);

  // Conectar ao Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("\n\nVersão v02.fodase");
  Serial.println("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  Serial.println("Connected to Wi-Fi");

  // Conectar ao servidor MQTT
  client.setServer(mqttServer, mqttPort);
  Serial.println("Connecting to MQTT");

  while (!client.connected()) {
    Serial.print(".");
    if (client.connect(mqttName.c_str(), mqttUser, mqttPassword)) {
      Serial.println("Connected to MQTT");
      sendMQTT_RegularA_DiscoveryMsg();
      sendMQTT_RegularB_DiscoveryMsg();
      sendMQTT_RegularC_DiscoveryMsg();
    } else {
      Serial.print("Failed with state: ");
      Serial.println(client.state());
      delay(2000);
    }
  }

  dht.begin();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("===== Sending Data =====");

    DynamicJsonDocument doc(1024);
    char buffer[256];

    doc["regulara"] = 61;
    doc["regularb"] = 62;
    doc["regularc"] = 63;

    size_t n = serializeJson(doc, buffer);
    
    bool published = client.publish(stateTopic.c_str(), buffer, n);
    if (published) {
      Serial.println("Data published successfully!");
    } else {
      Serial.println("Failed to publish data.");
    }

    // Importante: Chame client.loop() para manter a conexão MQTT viva
    client.loop();

    // Aguarde brevemente para garantir que as mensagens sejam enviadas antes de entrar em deep sleep
    delay(1000);
  } else {
    Serial.println("WiFi Disconnected");
  }

  // Go into deep sleep mode for 60 seconds
  Serial.println("Deep sleep mode for 60 seconds");
  ESP.deepSleep(10e6);
}

void sendMQTT_RegularA_DiscoveryMsg() {
  String discoveryTopic = "homeassistant/sensor/generic_sensor_" + String(sensorNumber) + "/regulara/config";

  DynamicJsonDocument doc(1024);
  char buffer[256];

  doc["name"] = "Generic " + String(sensorNumber) + " Regular A";
  doc["stat_t"] = stateTopic;
  doc["unit_of_meas"] = "°C";
  doc["dev_cla"] = "temperature";
  doc["frc_upd"] = true;
  doc["val_tpl"] = "{{ value_json.regulara|default(0) }}";
  doc["expire_after"] = 60;  // Sensor inativo após 60 segundos

  size_t n = serializeJson(doc, buffer);
  client.publish(discoveryTopic.c_str(), buffer, n);
  Serial.println("Publicando Regular A");

  client.loop();  // Certifique-se de que o loop é chamado após a publicação
  delay(100);  // Pequeno atraso para garantir que a mensagem seja enviada
}

void sendMQTT_RegularB_DiscoveryMsg() {
  String discoveryTopic = "homeassistant/sensor/generic_sensor_" + String(sensorNumber) + "/regularb/config";

  DynamicJsonDocument doc(1024);
  char buffer[256];

  doc["name"] = "Generic " + String(sensorNumber) + " Regular B";
  doc["stat_t"] = stateTopic;
  doc["unit_of_meas"] = "%";
  doc["dev_cla"] = "humidity";
  doc["frc_upd"] = true;
  doc["val_tpl"] = "{{ value_json.regularb|default(0) }}";
  doc["expire_after"] = 60;  // Sensor inativo após 60 segundos

  size_t n = serializeJson(doc, buffer);
  client.publish(discoveryTopic.c_str(), buffer, n);
  Serial.println("Publicando B");

  client.loop();
  delay(100);
}

void sendMQTT_RegularC_DiscoveryMsg() {
  String discoveryTopic = "homeassistant/sensor/generic_sensor_" + String(sensorNumber) + "/regularc/config";

  DynamicJsonDocument doc(1024);
  char buffer[256];

  doc["name"] = "Generic " + String(sensorNumber) + " Regular C";
  doc["stat_t"] = stateTopic;
  doc["frc_upd"] = true;
  doc["val_tpl"] = "{{ value_json.regularc|default(0) }}";
  doc["expire_after"] = 60;  // Sensor inativo após 60 segundos

  size_t n = serializeJson(doc, buffer);
  client.publish(discoveryTopic.c_str(), buffer, n);

  Serial.println("Publicando Regular C");

  client.loop();
  delay(100);
}

void loop() {}
