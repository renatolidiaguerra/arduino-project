
// DOIT ESP32 DEVKIT V1

// alternativa: lolin wemos d1 r1
//

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
const char* topicControl = "home/generic/cortinasala/control";

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

int controle_voltas = 0;
int controle_voltas_max = 20;
int controle_posicao = 0;

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
  controle_voltas = preferences.getUInt("counter", 0);
  Serial.print("memoria.controle_voltas: ");
  Serial.println(controle_voltas);
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

  // Serial.print("memoria.controle_voltas: ");
  // Serial.println(controle_voltas);

  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(callback);

  if (!mqttClient.connected()) {
    reconnect();
  }

  mqttClient.subscribe(topicControl);  // Subscrever ao tópico para receber comandos

  sendMqttDiscoveryPosition();
  sendMqttDiscoveryConnect();

  //publish_topicPosition(controle_voltas);

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
    doc["position"] = controle_voltas;

    size_t n = serializeJson(doc, buffer);

    published = mqttClient.publish(topicState.c_str(), buffer, n);
    if (!published) {
      Serial.println("Failed to publish data.");
    }

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
      mqttClient.subscribe(topicControl);  // Subscrever ao tópico para receber comandos

    } else {
      Serial.print("Failed, rc=");
      Serial.println(mqttClient.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
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

  //	0: mover somando parametro
  //	1: mover para posicao
  //	2: mover 1 posicao para ajuste fino (+200/-200)
  //	3:
  //	4:
  //	5:
  //	6: velocidade
  // 	7: seta valor atual / posicao atual
  //  8: valor maximo
  //  9: comunicação servidor (991; 992; 993; 999)

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

    int qtde_voltas = 0;

    // // Converter os caracteres ASCII para números inteiros e compor o valor final
    // for (int i = 1; i < 4; i++) {
    //     qtde_voltas = qtde_voltas * 10 + (payload[i] - '0');
    // }

    // comunicacao com servidor
    if (message == "\"RESTART\"") {
      Serial.println("< ========================= > restartando < ========================= > ");
      ESP.restart();
      return;
    }
    // comunicacao com servidor
    // if (message == "\"STATUS\"") {
    //   // mqttClient.publish(topicState, "online");

    //   publish_topicPosition(controle_voltas);

    //   Serial.print("!!");
    //   return;
    // }

    // recebe valor ATUAL! via topico
    if (qtde_voltas >= 700 && qtde_voltas <= 799) {
      controle_voltas = qtde_voltas - 700;

      preferences.putUInt("counter", controle_voltas);

      // mqttClient.publish(topicState, "done:x799");
      // publish_topicPosition(controle_voltas);

      Serial.print("  -  controle_voltas: ");
      Serial.print(controle_voltas);
      Serial.println("  -  Sending: +799");
      first_run = false;
      return;
    }

    // ajuste fino via 1 volta
    if (message == "\"AJUSTE+\"") {
      voltas(1, +1);
      // mqttClient.publish(topicState, "done:x200");
      Serial.println("...ajustando...");
      return;
    }
    // ajuste fino via 1 volta
    if (message == "\"AJUSTE-\"") {
      voltas(1, -1);
      // mqttClient.publish(topicState, "done:x200");
      Serial.println("...ajustando...");
      return;
    }

    if (message == "\"POSICAO0\"") {
      controle_posicao = 0;
    } else {
      if (message == "\"POSICAO5\"") {
        controle_posicao = 5;
      } else {
        if (message == "\"POSICAO10\"") {
          controle_posicao = 10;
        } else {
          if (message == "\"POSICAO15\"") {
            controle_posicao = 15;
          } else {
            if (message == "\"POSICAO20\"") {
              controle_posicao = 20;
            } else {
              Serial.println("mensagem não identificada");
              return;
            }
          }
        }
      }
    }

    // recebe valor POSICIONAL via topico
    // if (qtde_voltas >= 100 && qtde_voltas <= 199) {
    //   controle_posicao = qtde_voltas - 100;
    Serial.print("controle_posicao atual:");
    Serial.print(controle_posicao);

    int posicao_desejada = 0;

    if (controle_posicao > controle_voltas) {
      if (controle_posicao > controle_voltas_max) {
        Serial.print("<-- valor excede; retorno sem açao");
        return;
      }
      posicao_desejada = controle_posicao - controle_voltas;
      Serial.print("  -->  posicao desejada (a):");
      Serial.print(posicao_desejada);

      voltas(posicao_desejada, +1);
      controle_voltas = controle_posicao;

      preferences.putUInt("counter", controle_voltas);
      Serial.print("  -->  controle_voltas (a): ");
      Serial.print(controle_voltas);
    } else {
      if (controle_posicao < controle_voltas) {

        posicao_desejada = controle_voltas - controle_posicao;
        Serial.print("  -->  posicao desejada (b): ");
        Serial.println(posicao_desejada);

        voltas((controle_voltas - controle_posicao), -1);
        controle_voltas = controle_posicao;

        preferences.putUInt("counter", controle_voltas);  // grava em memoria posicao atual

        Serial.print("  -->  controle_voltas (b): ");
        Serial.print(controle_voltas);
      } else {
        Serial.print("  <--  valores iguais; retorno sem açao");
      }
    }
    // mqttClient.publish(topicState, "done:x100");
    publish_topicPosition(controle_voltas);
    return;
  }
}

void voltas(int qtde, int direcao)  // 1 = direita; -1 = esquerda
{
  int i = 0;
  int sentido = -1;

  // liga motor
  Serial.print(" ... motor ");

  // digitalWrite(motorPin1,HIGH);
  // digitalWrite(motorPin2,HIGH);
  // digitalWrite(motorPin3,HIGH);
  // digitalWrite(motorPin4,HIGH);

  int aux_posicao = controle_voltas;

  Serial.print("inicial aux_posicao:");
  Serial.println(aux_posicao);
  Serial.print(" ->");

  for (i = 0; i < qtde; i++) {
    // myStepper.step(stepsPerRevolution * direcao);

    if (direcao > 0) {
      // soma
      aux_posicao++;
    } else {
      // subtrai
      aux_posicao--;
    }

    // grava posicao temporaria em memoria
    preferences.putUInt("counter", aux_posicao);

    delay(delay_motor);
    mqttClient.loop();
  }
  // desliga motor
  // digitalWrite(motorPin1,LOW);
  // digitalWrite(motorPin2,LOW);
  // digitalWrite(motorPin3,LOW);
  // digitalWrite(motorPin4,LOW);

  Serial.print("[ok]");

  Serial.print("final   aux_posicao:");
  Serial.println(aux_posicao);
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

  Serial.print("Gravando posicao:");
  Serial.println(valor);
}

void sendMqttDiscoveryPosition() {

  DynamicJsonDocument doc(1024);
  char buffer[256];

  doc["name"] = "Cortina Sala - Posição";
  doc["stat_t"] = topicState;
  doc["unit_of_meas"] = "p";
  doc["dev_cla"] = "power_factor";  //"power_factor";
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
  // doc["val_tpl"] = "{{ value_json.state | is_defined }}";  // Template para interpretar o estado como booleano
  doc["val_tpl"] = "{{ value_json.state | \"OFF\" }}";  // Template para interpretar o estado como booleano
  doc["expire_after"] = 60;                             // O sensor será considerado inativo após 60 segundos sem atualizações

  size_t n = serializeJson(doc, buffer);
  mqttClient.publish(discoveryTopicConnect.c_str(), buffer, n);
  Serial.println("Publicando Discovery Connect");

  mqttClient.loop();
  delay(100);
}