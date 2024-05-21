#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <Stepper.h>

#include <iostream>
#include <cstring>

// Informações do WiFi
#define WIFISSID "warbrito"
#define WIFIPASS "gb240820"

// Informações do Servidor MQTT
const char* mqttServer = "192.168.15.10";
const int mqttPort = 1883;
const char* mqttUser = "renatobrito";
const char* mqttPass = "gb240820";
const char* topicControl = "homeassistant/esp32/modelo/control";
const char* topicState   = "homeassistant/esp32/modelo/state";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Declarar variaveis  dos sensores

void setup() {
  Serial.begin(115200);
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
  String payloadConfig = "{\"name\": \"Modelo01\", \"command_topic\": \"homeassistant/esp32/modelo/control\", \"state_topic\": \"homeassistant/esp32/modelo/state\"}";
  mqttClient.publish("homeassistant/esp32/modelo/config", payloadConfig.c_str(), true);
}

void loop() {
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop(); // Manter a conexão MQTT ativa
  delay(100); // Pequeno atraso para evitar rebotes no botão
}

void reconnect() {
  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT...");
    if (mqttClient.connect("Modelo01", mqttUser, mqttPass)) {
      Serial.println("Connected to MQTT");
      mqttClient.publish(topicState, "online");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
      contador_restart += 1;
      if (contador_restart > 12) {
        esp_restart();
      }
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {

  //	0: mover somando parametro
  //	1: mover para posicao
	//	2: mover 1 posicao para ajuste fino (+200/-200)
	//	3:
	//	4:
	//	5:
  //	6: velocidade
  // 	7: seta valor atual / posicao atual
  //  8: valor maximo
  //  9: comunicação servidor


  int qtde_voltas = 0;

  // Converter os caracteres ASCII para números inteiros e compor o valor final
  for (int i = 1; i < 4; i++) {
      qtde_voltas = qtde_voltas * 10 + (payload[i] - '0');
  }

  // comunicacao com servidor
  if (qtde_voltas == 991) {
    mqttClient.publish(topicState, "online");
    Serial.print("%");
    return;
  }

  if (qtde_voltas == 992) {
    mqttClient.publish(topicState, "done:x992");
    return;
  }

  Serial.println("Parametro não reconhecido!");

}
