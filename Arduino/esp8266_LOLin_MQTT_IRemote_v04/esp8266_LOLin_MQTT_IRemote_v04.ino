#include <IRrecv.h>
#include <IRremoteESP8266.h>

IRrecv irrecv(D5);
decode_results results;

// ------
#include <EEPROM.h>  // Inclua a EEPROM para persistir o estado
// ------ 

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char *WIFI_SSID = "warbrito";
const char *WIFI_PASSWORD = "gb240820";

const char* mqttServer = "192.168.15.10";  // O IP do seu broker MQTT
const int mqttPort = 1883;
const char* mqttUser = "renatobrito";
const char* mqttPassword = "gb240820";

String mqttName     = "IReceiver";
String stateTopic   = "home/generic/ireceiver/state";   
String commandTopic = "home/generic/ireceiver/command";  // Novo tópico de comando para callback

String command4Topic = "home/generic/ireceiver/4/command";  // Novo tópico de comando para callback
String command5Topic = "home/generic/ireceiver/5/command";  // Novo tópico de comando para callback
String command6Topic = "home/generic/ireceiver/6/command";  // Novo tópico de comando para callback
String command7Topic = "home/generic/ireceiver/7/command";  // Novo tópico de comando para callback
String command8Topic = "home/generic/ireceiver/8/command";  // Novo tópico de comando para callback
String command9Topic = "home/generic/ireceiver/9/command";  // Novo tópico de comando para callback
String command0Topic = "home/generic/ireceiver/0/command";  // Novo tópico de comando para callback

bool ireceiver_4_state = false;
bool ireceiver_5_state = false;
bool ireceiver_6_state = false;
bool ireceiver_7_state = false;
bool ireceiver_8_state = false;
bool ireceiver_9_state = false;
bool ireceiver_0_state = false;

bool IRrecebido = true;

DynamicJsonDocument doc(1024);

WiFiClient wifiClient;
PubSubClient client(wifiClient);

const unsigned long delay_resposta_mqtt = 59000;
unsigned long lastMessageTime = 0;

void setup() {

  irrecv.enableIRIn();  // Start the receiver
  Serial.begin(9600);

  EEPROM.begin(512);  // Inicializa a EEPROM
  //recuperaEstadoEEPROM();  // Recupera os estados ao iniciar

  // Conectar ao Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("\n\n===============Versão v18:17 - IReceiver\n===============");
  Serial.println("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  Serial.println("Connected to Wi-Fi");

  // Configura o servidor e o callback MQTT
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);  // Define a função de callback
  Serial.println("Connecting to MQTT");

  while (!client.connected()) {
    Serial.print(".");
    if (client.connect(mqttName.c_str(), mqttUser, mqttPassword)) {

      Serial.println("Connected to MQTT");
      client.subscribe(commandTopic.c_str());  // Subscreve o tópico de comando para receber mensagens
      client.subscribe(command4Topic.c_str());  // Subscreve o tópico de comando para receber mensagens
      client.subscribe(command5Topic.c_str());  // Subscreve o tópico de comando para receber mensagens
      client.subscribe(command6Topic.c_str());  // Subscreve o tópico de comando para receber mensagens
      client.subscribe(command7Topic.c_str());  // Subscreve o tópico de comando para receber mensagens
      client.subscribe(command8Topic.c_str());  // Subscreve o tópico de comando para receber mensagens
      client.subscribe(command9Topic.c_str());  // Subscreve o tópico de comando para receber mensagens
      client.subscribe(command0Topic.c_str());  // Subscreve o tópico de comando para receber mensagens
    //Serial.println("fazendo o requestState");

      // delay(15000);
      // requestStateFromMQTT();
      //Serial.println("request feito!");
      // delay(15000);

      // sendMQTT_IReceiver_switch_DiscoveryMsg(4);
      // sendMQTT_IReceiver_switch_DiscoveryMsg(5);
      // sendMQTT_IReceiver_switch_DiscoveryMsg(6);
      // sendMQTT_IReceiver_switch_DiscoveryMsg(7);
      // sendMQTT_IReceiver_switch_DiscoveryMsg(8);
      // sendMQTT_IReceiver_switch_DiscoveryMsg(9);
      // sendMQTT_IReceiver_switch_DiscoveryMsg(0);

      //sendMQTT_values();
      // requestStateFromMQTT();

      delay(200);
    } else {
      Serial.print("Failed with state: ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}


// The repeating section of the code
void loop() {

  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
      reconnect();
    }

    if (irrecv.decode(&results)) {
      IRrecebido = recebeIR();
      irrecv.resume();
    }

    unsigned long now = millis();
    if (now - lastMessageTime > delay_resposta_mqtt  ||  IRrecebido) {

      lastMessageTime = now;
      IRrecebido = false;

      char buffer[256];

      size_t n = serializeJson(doc, buffer);
      bool published = client.publish(stateTopic.c_str(), buffer, n);

      if (published) {
        Serial.println("Data published successfully! (loop)");
      } else {
        Serial.println("Failed to publish data.");
      }
    }
    // Importante: Chame client.loop() para manter a conexão MQTT viva e processar callbacks
    client.loop();
    delay(100);

  } else {
    Serial.println("WiFi Disconnected");
  }
}


void reconnect() {

  while (!client.connected()) {
    Serial.print("Connecting to MQTT... ");
    if (client.connect(mqttName.c_str(), mqttUser, mqttPassword)) {
      Serial.println("Connected!");
    } else {
      Serial.print("Failed, rc=");
      Serial.println(client.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

bool recebeIR() {
  // Check if the IR code has been received.
    bool retorno = false;
    Serial.print("Comando via IR: ");
    Serial.print(results.value, HEX);

    if (results.value == 0xFFA25D) {   // 1
      Serial.print("=1");
    } 
    if (results.value == 0xFF629D) {   // 1
      Serial.print("=2");
    } 
    if (results.value == 0xFFE21D) {   // 
      Serial.print("=3");
    } 
    if (results.value == 0xFF22DD) {   // 
      ireceiver_4_state = !ireceiver_4_state;
      retorno = true;
      Serial.print("=4");
    } 
    if (results.value == 0xFF02FD) {   //
      ireceiver_5_state = !ireceiver_5_state;
      retorno = true;
      Serial.print("=5");
    } 
    if (results.value == 0xFFC23D) {   // 
      ireceiver_6_state = !ireceiver_6_state;
      retorno = true;
      Serial.print("=6");
    } 
    if (results.value == 0xFFE01F) {   // 
      ireceiver_7_state = !ireceiver_7_state;
      retorno = true;
      Serial.print("=7");
    } 
    if (results.value == 0xFFA857) {   // 
      ireceiver_8_state = !ireceiver_8_state;
      retorno = true;
      Serial.print("=8");
    } 
    if (results.value == 0xFF906F) {   // 
      ireceiver_9_state = !ireceiver_9_state;
      retorno = true;
      Serial.print("=9");
    } 
    if (results.value == 0xFF9867) {   // 
      ireceiver_0_state = !ireceiver_0_state;
      retorno = true;
      Serial.print("=0");
    } 
    if (results.value == 0xFFB04F) {   // 
      Serial.print("=#");
    } 
    if (results.value == 0xFF38C7) {   // 
      Serial.print("=ok");
    } 
    if (results.value == 0xFF18E7) {   // up
      Serial.print("=up");
    }
    if (results.value == 0xFF4AB5) {   // down
      Serial.print("=down");
    }        
    if (results.value == 0xFF10EF) {   // left
      Serial.print("=left");
    }    
    if (results.value == 0xFF5AA5) {   // right
      Serial.print("=right");
    }    
    if (results.value == 0xFF6897) {   // estrelas
      Serial.print("=star");
    }
    Serial.println("");
    delay(100);

    if (retorno) {
      doc["ireceiver_4"] = ireceiver_4_state ? "ON" : "OFF"; 
      doc["ireceiver_5"] = ireceiver_5_state ? "ON" : "OFF"; 
      doc["ireceiver_6"] = ireceiver_6_state ? "ON" : "OFF"; 
      doc["ireceiver_7"] = ireceiver_7_state ? "ON" : "OFF"; 
      doc["ireceiver_8"] = ireceiver_8_state ? "ON" : "OFF"; 
      doc["ireceiver_9"] = ireceiver_9_state ? "ON" : "OFF"; 
      doc["ireceiver_0"] = ireceiver_0_state ? "ON" : "OFF";

      // salvaEstadoEEPROM();  // Salvar após alteração
    }

    return(retorno);
}


// Função para salvar estados na EEPROM
void salvaEstadoEEPROM() {
  EEPROM.write(0, ireceiver_4_state);
  EEPROM.write(1, ireceiver_5_state);
  EEPROM.write(2, ireceiver_6_state);
  EEPROM.write(3, ireceiver_7_state);
  EEPROM.write(4, ireceiver_8_state);
  EEPROM.write(5, ireceiver_9_state);
  EEPROM.write(6, ireceiver_0_state);
  EEPROM.commit();  // Grava os valores na EEPROM
}

// Função para recuperar estados da EEPROM ao iniciar
void recuperaEstadoEEPROM() {
  ireceiver_4_state = EEPROM.read(0);
  ireceiver_5_state = EEPROM.read(1);
  ireceiver_6_state = EEPROM.read(2);
  ireceiver_7_state = EEPROM.read(3);
  ireceiver_8_state = EEPROM.read(4);
  ireceiver_9_state = EEPROM.read(5);
  ireceiver_0_state = EEPROM.read(6);
}

// Função para solicitar o estado do MQTT ao iniciar
// void requestStateFromMQTT() {
//   client.publish(commandTopic.c_str(), "REQUEST_STATE");
// }

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida no tópico: ");
  Serial.println(topic);

  // Converte o payload em uma string
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
 // Serial.print("payload:");
 // Serial.println(String(payload));

  // recebeu comando on/off para ireceiver 4: command4Topic; state4Topic(?)
  if (String(topic) == command4Topic) {
      ireceiver_4_state = !ireceiver_4_state;
      // sendMQTT_values();
      // IRrecebido = true;
  }
  if (String(topic) == command5Topic) {
      ireceiver_5_state = !ireceiver_5_state;
      // sendMQTT_values();
      // IRrecebido = true;
  }
  if (String(topic) == command6Topic) {
      ireceiver_6_state = !ireceiver_6_state;
      // sendMQTT_values();
      // IRrecebido = true;
  }
  if (String(topic) == command7Topic) {
      ireceiver_7_state = !ireceiver_7_state;
      // sendMQTT_values();
      // IRrecebido = true;
  }
  if (String(topic) == command8Topic) {
      ireceiver_8_state = !ireceiver_8_state;
      // sendMQTT_values();
      // IRrecebido = true;
  }
  if (String(topic) == command9Topic) {
      ireceiver_9_state = !ireceiver_9_state;
      // sendMQTT_values();
      // IRrecebido = true;
  }
  if (String(topic) == command0Topic) {
      ireceiver_0_state = !ireceiver_0_state;
      // sendMQTT_values();
      // IRrecebido = true;
  }
  sendMQTT_values();
  IRrecebido = true;
  
  // // recebeu comando resultado do REQUEST_STATE para GERAL
  // if (String(topic) == stateTopic) {
  //   // Processar mensagem de estado
  //   StaticJsonDocument<256> stateDoc;
  //   deserializeJson(stateDoc, message);
  //   Serial.println("depois do deserialize");
  //   Serial.println(String(stateDoc["ireceiver_4"]));
  //   Serial.println(String(stateDoc["ireceiver_5"]));
  //   Serial.println(String(stateDoc["ireceiver_6"]));
    
  //   // Atualizar variáveis de estado com os valores recebidos
  //   ireceiver_4_state = (stateDoc["ireceiver_4"] == "ON");
  //   ireceiver_5_state = (stateDoc["ireceiver_5"] == "ON");
  //   ireceiver_6_state = (stateDoc["ireceiver_6"] == "ON");
  //   ireceiver_7_state = (stateDoc["ireceiver_7"] == "ON");
  //   ireceiver_8_state = (stateDoc["ireceiver_8"] == "ON");
  //   ireceiver_9_state = (stateDoc["ireceiver_9"] == "ON");
  //   ireceiver_0_state = (stateDoc["ireceiver_0"] == "ON");

  //   Serial.println("Estados restaurados do MQTT!");
  // }

  // Lógica para lidar com o comando "REQUEST_STATE"
  // if (String(topic) == commandTopic && message == "REQUEST_STATE") {
  //   sendMQTT_values();  // Enviar o estado atual ao broker MQTT
  // }
}

void sendMQTT_values() {

    doc["ireceiver_4"] = ireceiver_4_state ? "ON" : "OFF"; 
    doc["ireceiver_5"] = ireceiver_5_state ? "ON" : "OFF"; 
    doc["ireceiver_6"] = ireceiver_6_state ? "ON" : "OFF"; 
    doc["ireceiver_7"] = ireceiver_7_state ? "ON" : "OFF"; 
    doc["ireceiver_8"] = ireceiver_8_state ? "ON" : "OFF"; 
    doc["ireceiver_9"] = ireceiver_9_state ? "ON" : "OFF"; 
    doc["ireceiver_0"] = ireceiver_0_state ? "ON" : "OFF"; 

    // Serial.println("dentro do sendMQTT_values:");
    
    // doc["ireceiver_n_6"] = 30; 

    // char buffer[256];
    // size_t n = serializeJson(doc, buffer);
    // bool published = client.publish(stateTopic.c_str(), buffer, n);

    // if (published) {
    //   Serial.println("Data published successfully! (sendValues)");
    // } else {
    //   Serial.println("Failed to publish data.");
    // }
    // client.loop();
    // delay(100);
}

void sendMQTT_IReceiver_DiscoveryMsg(int indice) {
  String discoveryTopic = "homeassistant/binary_sensor/ireceiver_" + String(indice) + "/config";

  DynamicJsonDocument doc(1024);
  char buffer[256];

  // Definindo o nome do dispositivo, o tópico de estado e outros atributos do binary_sensor
  doc["name"] = "IReceiver " + String(indice);
  doc["stat_t"] = stateTopic;  // Tópico de estado para o binary_sensor
  doc["dev_cla"] = "connectivity";  // Classe de dispositivo: ajustável conforme o propósito
  doc["frc_upd"] = true;  // Força a atualização do estado
  doc["val_tpl"] = "{{ value_json.ireceiver_" + String(indice) + " | is_defined }}";  // Template para interpretar o estado como booleano
  doc["expire_after"] = 60;  // O sensor será considerado inativo após 60 segundos sem atualizações

  size_t n = serializeJson(doc, buffer);
  client.publish(discoveryTopic.c_str(), buffer, n);

  client.loop();
  delay(100);
}

void sendMQTT_IReceiver_switch_DiscoveryMsg(int indice) {
  String discoveryTopic = "homeassistant/switch/ireceiver_" + String(indice) + "/config";

  Serial.println("criando switch");
  DynamicJsonDocument doc(1024);
  char buffer[256];

  doc["name"]    = "IReceiver " + String(indice);
  doc["stat_t"]  = stateTopic;  // Tópico de estado para o switch
  doc["cmd_t"]   = commandTopic;  // Tópico de comando para alterar o estado do switch
  doc["frc_upd"] = true;
  doc["val_tpl"] = "{{ value_json.ireceiver_" + String(indice) + " | is_defined }}";  // Template para interpretar o estado como booleano
  doc["pl_on"]   = "ON";  // Payload que representa o estado ligado
  doc["pl_off"]  = "OFF";  // Payload que representa o estado desligado
  doc["expire_after"] = 60;  // Sensor inativo após 60 segundos

  size_t n = serializeJson(doc, buffer);
  client.publish(discoveryTopic.c_str(), buffer, n);

  client.loop();
  delay(100);
}

// void sendMQTT_IReceiver_n_DiscoveryMsg() {
//   String discoveryTopic = "homeassistant/input_number/IReceiver/config";

//   DynamicJsonDocument doc(1024);
//   char buffer[256];

//   // Definindo o nome do dispositivo e outros atributos
//   doc["name"] = "IReceiver n";
//   doc["stat_t"] = stateTopic;  // Tópico de estado para o input_number
//   // doc["min"] = 0;  // Valor mínimo
//   // doc["max"] = 100;  // Valor máximo
//   // doc["step"] = 1;  // Passo de ajuste
//   // doc["mode"] = "slider";  // Modo de ajuste
//   doc["frc_upd"] = true;  // Força a atualização
//   doc["val_tpl"] = "{{ value_json.ireceiver|default(0) }}";  // Template para o valor do estado
//   doc["expire_after"] = 60;  // Tempo de expiração

//   size_t n = serializeJson(doc, buffer);
//   client.publish(discoveryTopic.c_str(), buffer, n);

//   client.loop();
//   delay(100);
// }
