#include <PubSubClient.h>
#include <WiFi.h>
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
const char* topicControl = "nodemcu/cortinasala/control";
const char* topicState = "nodemcu/cortinasala/state";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Defina os pinos para o controle do motor de passo
const int stepsPerRevolution = 2048; // O motor de passo 28BYJ-48 tem 2048 passos por revolução

const int motorPin1 = 13;    // Pino IN1 conectado ao D5
const int motorPin2 = 12;    // Pino IN2 conectado ao D6
const int motorPin3 = 14;    // Pino IN3 conectado ao D7
const int motorPin4 = 27;    // Pino IN4 conectado ao D8

int qtde = 4;
int i = 0;
int valor = 2048;
int controle = 0;
int velocidade = 12;
int delay_motor = 5;

// Crie um objeto stepper
Stepper myStepper(stepsPerRevolution, motorPin1, motorPin3, motorPin2, motorPin4);

void setup() {
  Serial.begin(115200);

  // Configure o motor para uma velocidade média
  myStepper.setSpeed(velocidade); // Ajuste conforme necessário para a velocidade desejada 
  // 10: minimo
  // 12: maior torque
  // 15: menor torque
  
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
  String payloadConfig = "{\"name\": \"CortinaSala\", \"command_topic\": \"nodemcu/cortinasala/control\", \"state_topic\": \"nodemcu/cortinasala/state\"}";
  mqttClient.publish("homeassistant/switch/nodemcu/cortinasala/config", payloadConfig.c_str(), true);

}

void loop() {
  if (!mqttClient.connected()) {
    reconnect();
  }
  // Serial.print(".");
  mqttClient.loop(); // Manter a conexão MQTT ativa

  delay(100); // Pequeno atraso para evitar rebotes no botão
}

void reconnect() {
  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT...");
    if (mqttClient.connect("CortinaSala", mqttUser, mqttPass)) {
      Serial.println("Connected to MQTT");
      mqttClient.publish(topicState, "Connected to MQTT");
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

  // Serial.println();

  int payload_final = 0;

  // Converter os caracteres ASCII para números inteiros e compor o valor final
  for (int i = 1; i < 4; i++) {
      payload_final = payload_final * 10 + (payload[i] - '0');
  }

  // Serial.print("valor final convertido: ");
  // Serial.print(payload_final);

  if (payload_final == 991) {
    mqttClient.publish(topicState, "+992");
    Serial.println(" Sending message: +992");
    return;
  }

  Serial.println(" ... moving motor");

  if (payload[0] == '-') {
    voltas(payload_final, -1);
  } else {
    voltas(payload_final, +1);
  }

}

void voltas(int qtde, int direcao)  // 1 = direita; -1 = esquerda
{
  int i = 0;
  int sentido = -1;
  // mqttClient.publish(topicState, "volta_inicio");
  // liga motor
  digitalWrite(motorPin1,HIGH);
  digitalWrite(motorPin2,HIGH);
  digitalWrite(motorPin3,HIGH);
  digitalWrite(motorPin4,HIGH);

  for (i = 0; i < qtde; i++) {
    myStepper.step(stepsPerRevolution * direcao);
    delay(delay_motor);
    mqttClient.loop();
  }
  // desliga motor
  digitalWrite(motorPin1,LOW);
  digitalWrite(motorPin2,LOW);
  digitalWrite(motorPin3,LOW);
  digitalWrite(motorPin4,LOW);

  // mqttClient.publish(topicState, "volta_fim");

}