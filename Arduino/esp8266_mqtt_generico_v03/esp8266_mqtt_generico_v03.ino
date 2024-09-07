#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// LOLIN WeMos D1 R1

const char *WIFI_SSID = "warbrito";
const char *WIFI_PASSWORD = "gb240820";

const char* mqttServer = "192.168.15.10";  // O IP do seu broker MQTT
const int mqttPort = 1883;
const char* mqttUser = "renatobrito";
const char* mqttPassword = "gb240820";

int sensorPin = A0;  // Pino do sensor analógico

int sensorNumber = 12;
String mqttName = "Generic sensor " + String(sensorNumber);
String stateTopic = "home/generic/" + String(sensorNumber) + "/state";   

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void setup() {
  Serial.begin(9600);

  // Conectar ao Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("\n\n===============Versão v03." + String(sensorNumber) + " Generic\n===============");
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
      sendMQTT_RegularD_DiscoveryMsg();
      sendMQTT_RegularE_DiscoveryMsg();
      sendMQTT_RegularF_DiscoveryMsg();
      // sendMQTT_RegularG_DiscoveryMsg();
      sendMQTT_RegularH_DiscoveryMsg();

      delay(200);
    } else {
      Serial.print("Failed with state: ");
      Serial.println(client.state());
      delay(2000);
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("===== Sending Data =====");

    DynamicJsonDocument doc(1024);
    char buffer[256];

    doc["regulara"] = true;
    doc["regularb"] = 62;
    doc["regularc"] = 63;
    doc["regulard"] = "OFF";  // true ? "ON" : "OFF";
    doc["regulare"] = 1;
    doc["regularf"] = "ON";   // true ? "ON" : "OFF";  // Exemplo de como enviar o estado "ON" ou "OFF" para o input_boolean
    // doc["regularg"] = 1;
    doc["regularh"] = 39;

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
  String discoveryTopic = "homeassistant/binary_sensor/generic_sensor_" + String(sensorNumber) + "/regulara/config";

  DynamicJsonDocument doc(1024);
  char buffer[256];

  doc["name"] = "Generic " + String(sensorNumber) + " Regular A";
  doc["stat_t"] = stateTopic;
  // doc["unit_of_meas"] = "°C";
  doc["dev_cla"] = "power";  
  doc["frc_upd"] = true;
  doc["val_tpl"] = "{{ value_json.regulara|default(false) | bool }}";
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
  // doc["unit_of_meas"] = "%";
  doc["dev_cla"] = "battery";
  doc["frc_upd"] = true;
  doc["val_tpl"] = "{{ value_json.regularc|default(0) }}";
  doc["expire_after"] = 60;  // Sensor inativo após 60 segundos

  size_t n = serializeJson(doc, buffer);
  client.publish(discoveryTopic.c_str(), buffer, n);

  Serial.println("Publicando Regular C");

  client.loop();
  delay(100);
}

// void sendMQTT_RegularD_DiscoveryMsg() {
//   String discoveryTopic = "homeassistant/switch/generic_sensor_" + String(sensorNumber) + "/regulard/config";

//   DynamicJsonDocument doc(1024);
//   char buffer[256];

//   doc["name"] = "Generic " + String(sensorNumber) + " Regular D";
//   doc["stat_t"] = stateTopic;  // Tópico de estado para o switch
//   doc["cmd_t"] = "home/generic/" + String(sensorNumber) + "/regulard/set";  // Tópico de comando para alterar o estado do switch
//   doc["frc_upd"] = true;

//   // Atualizar o template de valor para verificar se o valor é igual a "ON" (então retorna true)
//   doc["val_tpl"] = "{{ value_json.regulard }}";  

//   // Define payloads para "ON" e "OFF"
//   doc["pl_on"] = "ON";  // Payload que representa o estado ligado
//   doc["pl_off"] = "OFF";  // Payload que representa o estado desligado
//   doc["expire_after"] = 60;  // Sensor inativo após 60 segundos

//   size_t n = serializeJson(doc, buffer);
//   client.publish(discoveryTopic.c_str(), buffer, n);

//   Serial.println("Publicando Regular D");

//   client.loop();
//   delay(100);
// }


void sendMQTT_RegularD_DiscoveryMsg() {
  String discoveryTopic = "homeassistant/switch/generic_sensor_" + String(sensorNumber) + "/regulard/config";

  DynamicJsonDocument doc(1024);
  char buffer[256];

  doc["name"] = "Generic " + String(sensorNumber) + " Regular D";
  doc["stat_t"] = stateTopic;  // Tópico de estado para o switch
  doc["cmd_t"] = "home/generic/" + String(sensorNumber) + "/set";  // Tópico de comando para alterar o estado do switch
  doc["frc_upd"] = true;
  doc["val_tpl"] = "{{ value_json.regulard|default(false) | bool }}";  // Corrigido o template de valor para o switch
  doc["pl_on"] = "ON";  // Payload que representa o estado ligado
  doc["pl_off"] = "OFF";  // Payload que representa o estado desligado
  doc["expire_after"] = 60;  // Sensor inativo após 60 segundos

  size_t n = serializeJson(doc, buffer);
  client.publish(discoveryTopic.c_str(), buffer, n);

  Serial.println("Publicando Regular D");

  client.loop();
  delay(100);
}

// void sendMQTT_RegularD_DiscoveryMsg_old() {
//   String discoveryTopic = "homeassistant/switch/generic_sensor_" + String(sensorNumber) + "/regulard/config";

//   DynamicJsonDocument doc(1024);
//   char buffer[256];

//   doc["name"] = "Generic " + String(sensorNumber) + " Regular D";
//   doc["stat_t"] = stateTopic;
//   // doc["unit_of_meas"] = "%";
//   doc["dev_cla"] = "is_on";
//   doc["frc_upd"] = true;
//   doc["val_tpl"] = "{{ value_json.regulard|default(false) | bool }}";
//   doc["expire_after"] = 60;  // Sensor inativo após 60 segundos

//   size_t n = serializeJson(doc, buffer);
//   client.publish(discoveryTopic.c_str(), buffer, n);

//   Serial.println("Publicando Regular D");

//   client.loop();
//   delay(100);
// }


void sendMQTT_RegularE_DiscoveryMsg() {
  String discoveryTopic = "homeassistant/sensor/generic_sensor_" + String(sensorNumber) + "/regulare/config";

  DynamicJsonDocument doc(1024);
  char buffer[256];

  doc["name"] = "Generic " + String(sensorNumber) + " Regular E";
  doc["stat_t"] = stateTopic;
  doc["unit_of_meas"] = "C";
  doc["dev_cla"] = "temperature";
  doc["frc_upd"] = true;
  doc["val_tpl"] = "{{ value_json.regulare|default(0) }}";
  doc["expire_after"] = 60;  // Sensor inativo após 60 segundos

  size_t n = serializeJson(doc, buffer);
  client.publish(discoveryTopic.c_str(), buffer, n);
  Serial.println("Publicando E");

  client.loop();
  delay(100);
}

void sendMQTT_RegularF_DiscoveryMsg() {
  String discoveryTopic = "homeassistant/binary_sensor/generic_sensor_" + String(sensorNumber) + "/regularf/config";

  DynamicJsonDocument doc(1024);
  char buffer[256];

  // Definindo o nome do dispositivo, o tópico de estado e outros atributos do binary_sensor
  doc["name"] = "Generic " + String(sensorNumber) + " Regular F";
  doc["stat_t"] = stateTopic;  // Tópico de estado para o binary_sensor
  doc["dev_cla"] = "motion";  // Classe de dispositivo: ajustável conforme o propósito
  doc["frc_upd"] = true;  // Força a atualização do estado
  doc["val_tpl"] = "{{ value_json.regularf|default(false) | bool }}";  // Template para o valor do estado
  
  // Adiciona um tempo de expiração para o sensor
  doc["expire_after"] = 60;  // O sensor será considerado inativo após 60 segundos sem atualizações

  size_t n = serializeJson(doc, buffer);
  client.publish(discoveryTopic.c_str(), buffer, n);

  Serial.println("Publicando Regular F como Binary Sensor");

  client.loop();
  delay(100);
}

void sendMQTT_RegularH_DiscoveryMsg() {
  String discoveryTopic = "homeassistant/input_number/generic_sensor_" + String(sensorNumber) + "/regularh/config";

  DynamicJsonDocument doc(1024);
  char buffer[256];

  // Definindo o nome do dispositivo e outros atributos
  doc["name"] = "Generic " + String(sensorNumber) + " Regular H";
  doc["stat_t"] = stateTopic;  // Tópico de estado para o input_number
  doc["min"] = 0;  // Valor mínimo
  doc["max"] = 100;  // Valor máximo
  doc["step"] = 1;  // Passo de ajuste
  doc["mode"] = "slider";  // Modo de ajuste
  doc["frc_upd"] = true;  // Força a atualização
  doc["val_tpl"] = "{{ value_json.regularh|default(0) }}";  // Template para o valor do estado
  doc["expire_after"] = 60;  // Tempo de expiração

  size_t n = serializeJson(doc, buffer);
  client.publish(discoveryTopic.c_str(), buffer, n);

  Serial.println("Publicando Regular H como Input Number");

  client.loop();
  delay(100);
}

void loop() {}
