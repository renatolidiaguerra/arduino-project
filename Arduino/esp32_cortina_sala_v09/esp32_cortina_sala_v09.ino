
// DOIT ESP32 DEVKIT V1
// alternativa: lolin wemos d1 r1

#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <Stepper.h>
#include <ArduinoJson.h>

#include <iostream>
#include <cstring>

#include <Preferences.h>

Preferences preferences;

// Informações do WiFi
#define WIFISSID "warbrito"
#define WIFIPASS "gb240820"

#define SLEEP_TIME 10e6  // Tempo de sono em microsegundos (1 minuto)

// Informações do Servidor MQTT
const char* mqttServer = "192.168.15.10";
const int mqttPort = 1883;
const char* mqttUser = "renatobrito";
const char* mqttPass = "gb240820";

String topicControl = "home/generic/cortinasala/control";
String discoveryTopicConnect = "homeassistant/binary_sensor/cortinasala/config";
String discoveryTopicPosition = "homeassistant/sensor/cortinasala/config";
String topicState = "home/generic/cortinasala/state";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Defina os pinos para o controle do motor de passo
const int stepsPerRevolution = 2048;  // O motor de passo 28BYJ-48 tem 2048 passos por revolução

const int motorPin1 = 13;  // Pino IN1 conectado ao D5
const int motorPin2 = 12;  // Pino IN2 conectado ao D6
const int motorPin3 = 14;  // Pino IN3 conectado ao D7
const int motorPin4 = 27;  // Pino IN4 conectado ao D8

int qtde = 4;
int i = 0;
int valor = 2048;
int controle = 0;
int velocidade = 12;
int delay_motor = 5;

int posicao_atual_motor = 0;
int maximo_voltas_motor = 20;
int posicao_desejada = 0;

const unsigned long delay_resposta_mqtt = 59000;
unsigned long lastMessageTime = 0;
unsigned long currentTime = 0;

char direcao_menos = 'd';
char direcao_mais = 'u';

bool first_run = true;
int contador_reboot = 0;

int cont_debug = 0;
int cont_debug_linha = 0;

int controle_marcopolo = 2;
int controle_loop = 0;

bool published = false;

// Crie um objeto stepper
Stepper myStepper(stepsPerRevolution, motorPin1, motorPin3, motorPin2, motorPin4);

void setup() {
  Serial.begin(9600);

  Serial.println("\n\n\n\n\n\nIniciando configuração...");
  Serial.println("Iniciando configuração...");
  Serial.println("Iniciando configuração...");

  // Configure o motor para uma velocidade média
  myStepper.setSpeed(velocidade);  // Ajuste conforme necessário para a velocidade desejada
  // 10: minimo
  // 12: maior torque
  // 15: menor torque

  // inicia memoria gravavel
  preferences.begin("memoria", false);
  // le memoria e atualiza controle de voltas atual
  posicao_atual_motor = preferences.getUInt("counter", 0);
  Serial.print("memoria.posicao_atual_motor: ");
  Serial.println(posicao_atual_motor);
  // finaliza gravacao memoria
  // preferences.end();

  WiFi.begin(WIFISSID, WIFIPASS);

  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println(" Connected to Wi-Fi");

  // Serial.print("memoria.posicao_atual_motor: ");
  // Serial.println(posicao_atual_motor);

  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(callback);

  if (!mqttClient.connected()) {
    reconnect();
  }

  mqttClient.subscribe(topicControl.c_str());  // Subscrever ao tópico para receber comandos

  sendMqttDiscoveryPosition();
  sendMqttDiscoveryConnect();

  //publish_topicPosition(posicao_atual_motor);

  Serial.println("\n Final Setup...");
  Serial.println("\n Inicio Loop");
}

void loop() {

  if (WiFi.status() == WL_CONNECTED) {

    if (!mqttClient.connected()) {
      reconnect();
    }

    DynamicJsonDocument doc(1024);
    char buffer[256];

    doc["state"] = true ? "ON" : "OFF";
    doc["position"] = posicao_atual_motor;

    size_t n = serializeJson(doc, buffer);

    published = mqttClient.publish(topicState.c_str(), buffer, n);
    if (!published) {
      Serial.println("Failed to publish data.");
    }

    Serial.print(":");
    mqttClient.loop();

    delay(100);

  } else {
    Serial.println("WiFi Disconnected");
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to WiFi...");
    }
  }
}

void reconnect() {

  while (!mqttClient.connected()) {

    Serial.print("Connecting to MQTT... ");

    if (mqttClient.connect("CortinaSala", mqttUser, mqttPass)) {
      Serial.println("Connected!");
      // mqttClient.subscribe(topicControl.c_str());  // Subscrever ao tópico para receber comandos

    } else {
      Serial.print("Failed, rc=");
      Serial.println(mqttClient.state());
      Serial.println(" Retrying in 1 second(s)...");
      delay(1000);
      contador_reboot += 1;
      Serial.print("fora do ar. Tentativa: ");
      Serial.println(contador_reboot);
      if (contador_reboot > 5) {
        Serial.println("< ========================= > restartando < ========================= > ");
        ESP.restart();
      }
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Msg recebida no tópico: ");
  Serial.println(topic);

  // Converte o payload em uma string
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Conteudo da mensagem  : ");
  Serial.println(message);

  if (String(topic) == topicControl) {

    if (message == "\"RESTART\"") {
      Serial.println("< ========================= > restartando < ========================= > ");
      ESP.restart();
      return;
    }

    // recebe valor ATUAL! via topico
    if (message.startsWith("\"SET")) {
      // Extrai os dois dígitos após "SET" e converte para um número inteiro
      int aux_set_posicao = message.substring(4).toInt(); // Pega substring a partir do índice 3 até o final
      Serial.print("Valor numérico extraído: ");
      Serial.println(aux_set_posicao);

      posicao_atual_motor = aux_set_posicao;

      preferences.putUInt("counter", posicao_atual_motor);

      Serial.print("  -  posicao_atual_motor: ");
      Serial.print(posicao_atual_motor);
      return;
    }

    // ajuste fino via 1 volta
    if (message == "\"AJUSTE+\"") {
      voltas(1, +1, false);
      // mqttClient.publish(topicState, "done:x200");
      Serial.println("...ajustando...");
      return;
    }
    // ajuste fino via 1 volta
    if (message == "\"AJUSTE-\"") {
      voltas(1, -1, false);
      // mqttClient.publish(topicState, "done:x200");
      Serial.println("...ajustando...");
      return;
    }

    if (message == "\"POSICAO0\"") {
      posicao_desejada = 0;
    } else {
      if (message == "\"POSICAO5\"") {
        posicao_desejada = 5;
      } else {
        if (message == "\"POSICAO10\"") {
          posicao_desejada = 10;
        } else {
          if (message == "\"POSICAO15\"") {
            posicao_desejada = 15;
          } else {
            if (message == "\"POSICAO20\"") {
              posicao_desejada = 20;
            } else {
              Serial.println("mensagem não identificada");
              return;
            }
          }
        }
      }
    }

    // recebe valor POSICIONAL via topico
    Serial.print("posicao_desejada atual:");
    Serial.print(posicao_desejada);

    int voltas_necessarias = 0;

    if (posicao_desejada > posicao_atual_motor) {
      if (posicao_desejada > maximo_voltas_motor) {
        Serial.print("<-- valor excede; retorno sem açao  ");
        return;
      }
      voltas_necessarias = posicao_desejada - posicao_atual_motor;
      Serial.print("  -->  voltas necessarias (a):");
      Serial.print(voltas_necessarias);

      voltas(voltas_necessarias, +1);
      posicao_atual_motor = posicao_desejada;

      preferences.putUInt("counter", posicao_atual_motor);
      Serial.print("  -->  posicao_atual_motor (a): ");
      Serial.print(posicao_atual_motor);
    } else {
      if (posicao_desejada < posicao_atual_motor) {

        voltas_necessarias = posicao_atual_motor - posicao_desejada;
        Serial.print("  -->  voltas_necessarias (b): ");
        Serial.println(voltas_necessarias);

        voltas((posicao_atual_motor - posicao_desejada), -1);
        posicao_atual_motor = posicao_desejada;

        preferences.putUInt("counter", posicao_atual_motor);  // grava em memoria posicao atual

        Serial.print("  -->  posicao_atual_motor (b): ");
        Serial.print(posicao_atual_motor);
      } else {
        Serial.print("  <--  valores iguais; retorno sem açao  ");
      }
    }
    publish_topicPosition(posicao_atual_motor);
    return;
  }
}

void voltas(int qtde_voltas, int direcao)  {
  voltas(qtde_voltas, direcao, true);
}

void voltas(int qtde_voltas, int direcao, bool write_memory)  // 1 = direita; -1 = esquerda
{
  int i = 0;
  int sentido = -1;

  // liga motor
  Serial.println("\n[ iniciando motor ]");

  // digitalWrite(motorPin1,HIGH);  // essencial
  // digitalWrite(motorPin2,HIGH);
  // digitalWrite(motorPin3,HIGH);
  // digitalWrite(motorPin4,HIGH);

  int aux_posicao_motor = posicao_atual_motor;

  Serial.print("aux_posicao_motor : ");
  Serial.print(aux_posicao_motor);
  Serial.print(" ->");

  for (i = 0; i < qtde_voltas; i++) {
    // myStepper.step(stepsPerRevolution * direcao);  // essencial

    if (direcao > 0) {
      // soma
      aux_posicao_motor++;
    } else {
      // subtrai
      aux_posicao_motor--;
    }

    // grava posicao temporaria em memoria
    if (write_memory) {
      preferences.putUInt("counter", aux_posicao_motor);
    }

    delay(delay_motor);
    //mqttClient.loop();
  }
  // desliga motor
  // digitalWrite(motorPin1,LOW);  // essencial
  // digitalWrite(motorPin2,LOW);
  // digitalWrite(motorPin3,LOW);
  // digitalWrite(motorPin4,LOW);

  Serial.println(aux_posicao_motor);
  Serial.println("[ finalizando motor ]");

}

void publish_topicPosition(int valor) {

  DynamicJsonDocument doc(1024);
  char buffer[256];

  doc["state"] = true ? "ON" : "OFF";
  doc["position"] = valor;

  size_t n = serializeJson(doc, buffer);

  published = mqttClient.publish(topicState.c_str(), buffer, n);
  if (!published) {
    Serial.println("Failed to publish data.");
  }

  mqttClient.loop();

  delay(100);

  Serial.print("  -->  Gravando posicao:");
  Serial.println(valor);
}

void sendMqttDiscoveryPosition() {

  DynamicJsonDocument doc(1024);
  char buffer[256];

  doc["name"] = "Cortina Sala - Posição";
  doc["stat_t"] = topicState;
  doc["unit_of_meas"] = "!";
  doc["dev_cla"] = "frequency";  //"power_factor";
  doc["frc_upd"] = true;
  doc["val_tpl"] = "{{ value_json.position|default(0) }}";
  doc["expire_after"] = 60;  // Sensor inativo após 60 segundos

  size_t n = serializeJson(doc, buffer);
  mqttClient.publish(discoveryTopicPosition.c_str(), buffer, n);

  Serial.println("Publicando Discovery Position");

  mqttClient.loop();
  delay(100);
}

void sendMqttDiscoveryConnect() {

  DynamicJsonDocument doc(1024);
  char buffer[256];

  doc["name"] = "Cortina Sala - Conexão";
  doc["stat_t"] = topicState;       // Tópico de estado para o binary_sensor
  doc["dev_cla"] = "connectivity";  // Classe de dispositivo: ajustável conforme o propósito
  doc["frc_upd"] = true;            // Força a atualização do estado
  doc["val_tpl"] = "{{ value_json.state | is_defined }}";  // Template para interpretar o estado como booleano
  doc["expire_after"] = 60;                             // O sensor será considerado inativo após 60 segundos sem atualizações

  size_t n = serializeJson(doc, buffer);
  mqttClient.publish(discoveryTopicConnect.c_str(), buffer, n);
  Serial.println("Publicando Discovery Connect");

  mqttClient.loop();
  delay(100);
}

// M                     