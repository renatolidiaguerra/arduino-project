#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Configurações de WiFi e MQTT
const char* WIFI_SSID = "warbrito";
const char* WIFI_PASSWORD = "gb240820";
const char* mqttServer = "192.168.15.10";  // IP do seu servidor MQTT
const int mqttPort = 1883;
const char* mqttUser = "renatobrito";
const char* mqttPassword = "gb240820";

// Configurações do sensor e MQTT
int sensorPin = A0;  // Pino analógico para leitura do sensor
int sensorNumber = 2;  // Número de identificação do sensor
String mqttName = "Sensor " + String(sensorNumber);
String stateTopic = "home/sensor/" + String(sensorNumber) + "/state";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void setup() {
  Serial.begin(9600);

  // Conectar ao Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  Serial.println("Conectado ao Wi-Fi");

  // Conectar ao MQTT
  client.setServer(mqttServer, mqttPort);
  Serial.println("Conectando ao MQTT");
  while (!client.connected()) {
    Serial.print(".");
    if (client.connect(mqttName.c_str(), mqttUser, mqttPassword)) {
      Serial.println("Conectado ao MQTT");
      sendMQTTDiscoveryMsg();
    } else {
      Serial.print("Falha na conexão MQTT, estado: ");
      Serial.println(client.state());
      delay(2000);
    }
  }

  // Ler o sensor e enviar dados MQTT
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("===== Enviando Dados =====");
    
    int sensorValue = analogRead(sensorPin);  // Ler valor do sensor
    sensorValue = 15;

    DynamicJsonDocument doc(256);
    char buffer[128];

    doc["sensorexterno"] = sensorValue;  // Armazena o valor do sensor no JSON

    size_t n = serializeJson(doc, buffer);
    client.publish(stateTopic.c_str(), buffer, n);  // Publicar os dados do sensor no MQTT

    Serial.println("Dados enviados!");
    Serial.print("Valor do sensor: ");
    Serial.println(sensorValue);
  } else {
    Serial.println("WiFi desconectado");
  }

  // Modo de deep sleep por 10 segundos
  Serial.println("Modo Deep Sleep por 10 segundos");
  ESP.deepSleep(10e6);
}

void sendMQTTDiscoveryMsg() {
  // Enviar mensagem de descoberta MQTT para Home Assistant
  String discoveryTopic = "homeassistant/sensor/novo_sensor/sensor_" + String(sensorNumber) + "/config";
  
  DynamicJsonDocument doc(256);
  char buffer[128];

  doc["name"] = "NovoSensor " + String(sensorNumber);
  doc["stat_t"] = stateTopic;
  doc["unit_of_meas"] = "%";  // Unidade de medida do sensor (pode ser adaptado conforme necessário)
  doc["dev_cla"] = "humidity";
  doc["frc_upd"] = true;
  // doc["val_tpl"] = "{{ value_json.sensorexterno|default(0) }}";
  // doc["val_tpl"] = "{{ value_json.moisture1|default(0) }}"
  doc["val_tpl"] = "{{ value_json.sensorexterno|default(0) }}";   // digitado à mão 
  // value_json.humidity|default(0) }}"}
  size_t n = serializeJson(doc, buffer);
  client.publish(discoveryTopic.c_str(), buffer, n);  // Publicar mensagem de descoberta no MQTT

  Serial.println("Publicando mensagem de descoberta para o sensor");
}

void loop() {}
