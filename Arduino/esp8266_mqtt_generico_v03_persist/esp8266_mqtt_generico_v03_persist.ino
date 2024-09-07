#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// LOLIN WeMos D1 R1

int var1 = 1;
int var2 = 2;
int var3 = 3;
int var5 = 5;
int var7 = 7;

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

      sendMQTT_RegularD_DiscoveryMsg(); // erro

      sendMQTT_RegularE_DiscoveryMsg();
      sendMQTT_RegularF_DiscoveryMsg();

      sendMQTT_RegularG_DiscoveryMsg(); // erro
      sendMQTT_RegularH_DiscoveryMsg(); // erro

      delay(200);
    } else {
      Serial.print("Failed with state: ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("===== Sending Data =====");

    DynamicJsonDocument doc(1024);
    char buffer[256];

    var2 += 2;
    var3 += 3;
    var5 += 5;
    var7 += 7;

    //doc["regulara"] = true ? "ON" : "OFF";


    doc["regularb"] = var2;
    doc["regularc"] = var3;

    var1 += 1;

    if (var1 > 5) {
      var1 = 0;
      doc["regulara"] = true ? "ON" : "OFF";
      doc["regularg"] = true ? "ON" : "OFF";
      doc["regularf"] = false ? "ON" : "OFF";
      doc["regulard"] = false ? "ON" : "OFF";
    } else {
      doc["regulara"] = false ? "ON" : "OFF";
      doc["regularg"] = false ? "ON" : "OFF";
      doc["regularf"] = true ? "ON" : "OFF";
      doc["regulard"] = true ? "ON" : "OFF";
    }
    // Serial.println(doc["regulard"]);

    doc["regulare"] = var5;
    // doc["regularf"] = true ? "ON" : "OFF";  // Exemplo de como enviar o estado "ON" ou "OFF" para o input_boolean
    
    // doc["regularg"] = 1;
    
    doc["regularh"] = var7;

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
  // Serial.println("Deep sleep mode for 60 seconds");
  // ESP.deepSleep(10e6);
}

void sendMQTT_RegularA_DiscoveryMsg() {
  // Define o tópico de descoberta para o binary sensor no Home Assistant
  String discoveryTopic = "homeassistant/binary_sensor/generic_sensor_" + String(sensorNumber) + "/regulara/config";

  // Criação do documento JSON para o payload da mensagem de descoberta
  DynamicJsonDocument doc(1024);
  char buffer[256];

  // Definição dos atributos obrigatórios e opcionais do sensor
  doc["name"] = "Generic " + String(sensorNumber) + " Regular A";  // Nome do sensor
  doc["stat_t"] = stateTopic;  // Tópico de estado do sensor
  doc["dev_cla"] = "light";  // Classe de dispositivo que define o ícone e o comportamento do sensor: heat
  doc["val_tpl"] = "{{ value_json.regulara | is_defined }}";  // Template para interpretar o estado como booleano
  doc["frc_upd"] = true;  // Força a atualização do estado mesmo que o valor seja o mesmo
  doc["expire_after"] = 60;  // O sensor será considerado inativo após 60 segundos sem atualizações

  // Serialização do documento JSON para o buffer de saída
  size_t n = serializeJson(doc, buffer);

  // Publicação da mensagem de descoberta no tópico MQTT
  client.publish(discoveryTopic.c_str(), buffer, n);
  Serial.println("Publicando Regular A");

  // Chamada ao loop do cliente MQTT para garantir que as mensagens sejam enviadas
  client.loop();
  
  // Pequeno atraso para garantir que a mensagem seja enviada antes de continuar
  delay(100);
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
  doc["dev_cla"] = "connectivity";  // Classe de dispositivo: ajustável conforme o propósito
  doc["frc_upd"] = true;  // Força a atualização do estado
  doc["val_tpl"] = "{{ value_json.regularf | is_defined }}";  // Template para interpretar o estado como booleano
  doc["expire_after"] = 60;  // O sensor será considerado inativo após 60 segundos sem atualizações

  size_t n = serializeJson(doc, buffer);
  client.publish(discoveryTopic.c_str(), buffer, n);

  Serial.println("Publicando Regular F como Binary Sensor");

  client.loop();
  delay(100);
}
// =========================================
// =========================================
// =========================================

void sendMQTT_RegularG_DiscoveryMsg() {
  // Define o tópico de descoberta para o switch no Home Assistant
  String discoveryTopic = "homeassistant/switch/generic_sensor_" + String(sensorNumber) + "/regularg/config";

  // Criação do documento JSON para o payload da mensagem de descoberta
  DynamicJsonDocument doc(1024);
  char buffer[256];

  // Definição dos atributos obrigatórios e opcionais do switch
  doc["name"] = "Generic " + String(sensorNumber) + " Regular G";  // Nome do switch
  doc["stat_t"] = stateTopic;  // Tópico de estado do switch
  doc["cmd_t"] = "home/generic/" + String(sensorNumber) + "/regularg/set";  // Tópico de comando para alterar o estado do switch
  doc["val_tpl"] = "{{ value_json.regularg }}";  // Template para interpretar o estado
  doc["frc_upd"] = true;  // Força a atualização do estado mesmo que o valor seja o mesmo
  doc["pl_on"] = "ON";  // Payload que representa o estado ligado
  doc["pl_off"] = "OFF";  // Payload que representa o estado desligado
  doc["expire_after"] = 60;  // O switch será considerado inativo após 60 segundos sem atualizações

  // Serialização do documento JSON para o buffer de saída
  size_t n = serializeJson(doc, buffer);

  // Publicação da mensagem de descoberta no tópico MQTT
  client.publish(discoveryTopic.c_str(), buffer, n);
  Serial.println("Publicando Regular G como Switch");

  // Chamada ao loop do cliente MQTT para garantir que as mensagens sejam enviadas
  client.loop();
  
  // Pequeno atraso para garantir que a mensagem seja enviada antes de continuar
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
  // Definindo o tópico de descoberta MQTT para o switch no Home Assistant
  String discoveryTopic = "homeassistant/switch/generic_sensor_" + String(sensorNumber) + "/regulard/config";
  
  // Definindo o tópico de estado e comando para o switch
  String stateTopic = "homeassistant/switch/generic_sensor_" + String(sensorNumber) + "/regulard/state";
  String commandTopic = "homeassistant/switch/generic_sensor_" + String(sensorNumber) + "/regulard/set";

  // Criando o documento JSON para o payload da mensagem de descoberta
  DynamicJsonDocument doc(1024);
  char buffer[256];

  // Definindo os atributos do switch para o Home Assistant
  doc["name"] = "Generic " + String(sensorNumber) + " Regular D";  // Nome do switch
  doc["stat_t"] = stateTopic;  // Tópico de estado do switch
  doc["cmd_t"] = commandTopic;  // Tópico de comando para controlar o switch
  doc["payload_on"] = "ON";  // Payload para ligar o switch
  doc["payload_off"] = "OFF";  // Payload para desligar o switch
  doc["value_template"] = "{{ value }}";  // Template para interpretar o estado como "ON" ou "OFF"
  doc["unique_id"] = "generic_sensor_" + String(sensorNumber) + "_regulard";  // ID único para o dispositivo
  
  // Serializando o documento JSON para o buffer de saída
  size_t n = serializeJson(doc, buffer);

  // Corrigindo o tipo de buffer para o tipo esperado pelo método publish
  // Fazendo cast de (const uint8_t*) para o segundo argumento
  client.publish(discoveryTopic.c_str(), (const uint8_t*)buffer, n, true);  // "true" para mensagem persistente
  Serial.println("Publicando Regular D para switch");

  // Certifique-se de que a mensagem seja enviada antes de continuar
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


//void loop() {}
