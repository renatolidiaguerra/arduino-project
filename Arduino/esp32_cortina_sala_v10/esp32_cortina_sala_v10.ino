#if defined(ESP8266)
  #include <ESP8266WiFi.h>  // Include para ESP8266
#elif defined(ESP32)
  #include <WiFi.h>         // Include para ESP32
#else
  #error "Placa não suportada! Utilize ESP8266 ou ESP32."
#endif

#include <PubSubClient.h>
#include <Stepper.h>
#include <ArduinoJson.h>
#include <Preferences.h>

// Informações do WiFi
#define WIFISSID "warbrito"
#define WIFIPASS "gb240820"

// Informações do Servidor MQTT
const char* mqttServer = "192.168.15.10";
const int mqttPort = 1883;
const char* mqttUser = "renatobrito";
const char* mqttPass = "gb240820";

const char* deviceName = "CortinaSala";
String discoveryTopicConnect  = "homeassistant/binary_sensor/cortinasala/config";
String discoveryTopicPosition = "homeassistant/sensor/cortinasala/config";
String topicState   = "home/generic/cortinasala/state";
String topicControl = "home/generic/cortinasala/control";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
Preferences preferences;

// Defina os pinos para o controle do motor de passo
const int stepsPerRevolution = 2048;
const int motorPin1 = 13;
const int motorPin2 = 12;
const int motorPin3 = 14;
const int motorPin4 = 27;

const unsigned long delay_resposta_mqtt = 59000;
unsigned long lastMessageTime = 0;

int posicao_atual_motor = 0;
int maximo_voltas_motor = 20;
int posicao_desejada = 0;

int qtde = 4;
int i = 0;
int valor = 2048;
int controle = 0;
int velocidade = 12;
int delay_motor = 5;

bool published = false;
Stepper myStepper(stepsPerRevolution, motorPin1, motorPin3, motorPin2, motorPin4);

unsigned long lastReconnectAttempt = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("\n\n\n=======\n\n\n");
  Serial.println("Iniciando configuração...");

  myStepper.setSpeed(12);  // Ajuste conforme necessário para a velocidade desejada

  preferences.begin("memoria", false);
  posicao_atual_motor = preferences.getUInt("counter", 0);
  Serial.print("memoria.posicao_atual_motor: ");
  Serial.println(posicao_atual_motor);

  WiFi.begin(WIFISSID, WIFIPASS);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println(" Connected to Wi-Fi");

  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(callback);

  lastReconnectAttempt = 0;
}

void loop() {
  if (!mqttClient.connected()) {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > 2000) {
      lastReconnectAttempt = now;
      if (reconnect()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {
    mqttClient.loop();
  }

  // Publicação periódica da posição do motor a cada 59 segundos
  unsigned long now = millis();
  if (now - lastMessageTime > delay_resposta_mqtt) {
    lastMessageTime = now;
    publish_topicPosition(posicao_atual_motor);
    Serial.print("!");
  }
  //Serial.print(":");
  delay(100);
}

bool reconnect() {
  if (mqttClient.connect(deviceName, mqttUser, mqttPass)) {
    Serial.println("Connected to MQTT!");
    mqttClient.subscribe(topicControl.c_str());  // Subscrever ao tópico para receber comandos
    sendMqttDiscoveryPosition();
    sendMqttDiscoveryConnect();
  } else {
    Serial.print("Failed to connect to MQTT, state: ");
    Serial.println(mqttClient.state());
    return false;
  }
  return true;
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Msg recebida no tópico: ");
  Serial.println(topic);
  Serial.print("Conteudo da mensagem  : ");
  Serial.println(message);

  if (String(topic) == topicControl) {
    if (message == "\"RESTART\"") {
      ESP.restart();
      return;
    }

    if (message.startsWith("\"SET")) {
      int aux_set_posicao = message.substring(4).toInt();
      Serial.print("Recebendo force :");
      Serial.println(posicao_atual_motor);
      posicao_atual_motor = aux_set_posicao;
      publish_topicPosition(posicao_atual_motor);
      preferences.putUInt("counter", posicao_atual_motor);
      return;
    }

    // ajuste fino via 1 volta
    if (message == "\"AJUST+\"") {
      voltas(1, +1, false);
      // mqttClient.publish(topicState, "done:x200");
      Serial.println("...ajustando...");
      return;
    }
    // ajuste fino via 1 volta
    if (message == "\"AJUST-\"") {
      voltas(1, -1, false);
      // mqttClient.publish(topicState, "done:x200");
      Serial.println("...ajustando...");
      return;
    }

    if (message == "\"POSITION0\"") {
      posicao_desejada = 0;
    } else {
      if (message == "\"POSITION5\"") {
        posicao_desejada = 5;
      } else {
        if (message == "\"POSITION10\"") {
          posicao_desejada = 10;
        } else {
          if (message == "\"POSITION15\"") {
            posicao_desejada = 15;
          } else {
            if (message == "\"POSITION20\"") {
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

  digitalWrite(motorPin1,HIGH);  // essencial
  digitalWrite(motorPin2,HIGH);
  digitalWrite(motorPin3,HIGH);
  digitalWrite(motorPin4,HIGH);

  int aux_posicao_motor = posicao_atual_motor;

  Serial.print("aux_posicao_motor : ");
  Serial.print(aux_posicao_motor);
  Serial.print(" ->");

  for (i = 0; i < qtde_voltas; i++) {
    myStepper.step(stepsPerRevolution * direcao);  // essencial

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
    mqttClient.loop();
  }
  // desliga motor
  digitalWrite(motorPin1,LOW);  // essencial
  digitalWrite(motorPin2,LOW);
  digitalWrite(motorPin3,LOW);
  digitalWrite(motorPin4,LOW);

  Serial.println(aux_posicao_motor);
  Serial.println("[ finalizando motor ]");

}

void publish_topicPosition(int valor) {
  DynamicJsonDocument doc(1024);
  char buffer[256];

  doc["state"] = "ON";
  doc["position"] = valor;

  size_t n = serializeJson(doc, buffer);

  published = mqttClient.publish(topicState.c_str(), buffer, n);
  if (!published) {
    Serial.println("Failed to publish data.");
  }
  mqttClient.loop();  // Certifique-se de processar loop após publicação
}

void sendMqttDiscoveryPosition() {
  DynamicJsonDocument doc(1024);
  char buffer[256];

  doc["name"] = "Cortina Sala - Posição";
  doc["stat_t"] = topicState;
  doc["unit_of_meas"] = "!";
  doc["dev_cla"] = "frequency";
  doc["frc_upd"] = true;
  doc["val_tpl"] = "{{ value_json.position|default(0) }}";
  doc["expire_after"] = 60;

  size_t n = serializeJson(doc, buffer);
  mqttClient.publish(discoveryTopicPosition.c_str(), buffer, n);
  Serial.println("Publicando Discovery Position");
}

void sendMqttDiscoveryConnect() {
  DynamicJsonDocument doc(1024);
  char buffer[256];

  doc["name"] = "Cortina Sala - Conexão";
  doc["stat_t"] = topicState;
  doc["dev_cla"] = "connectivity";
  doc["frc_upd"] = true;
  doc["val_tpl"] = "{{ value_json.state | is_defined }}";
  doc["expire_after"] = 60;

  size_t n = serializeJson(doc, buffer);
  mqttClient.publish(discoveryTopicConnect.c_str(), buffer, n);
  Serial.println("Publicando Discovery Connect");
}

