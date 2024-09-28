#include <IRrecv.h>
#include <IRremoteESP8266.h>

IRrecv irrecv(D5);
decode_results results;

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

bool ireceiver_4_state = false;
bool ireceiver_5_state = false;

DynamicJsonDocument doc(1024);

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void setup() {

  irrecv.enableIRIn();  // Start the receiver
  Serial.begin(9600);

  // Conectar ao Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("\n\n===============Versão v04.IReceiver\n===============");
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

      sendMQTT_IReceiver_io_DiscoveryMsg(4);
      sendMQTT_IReceiver_io_DiscoveryMsg(5);

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
    //Serial.println("===== Sending Data =====");

    if (!client.connected()) {
      reconnect();
    }

    // DynamicJsonDocument doc(1024);
    char buffer[256];


    if (irrecv.decode(&results)) {
      recebeIR();
      irrecv.resume();
    }

    // doc["ireceiver_4"] = true ? "ON" : "OFF";
    // doc["ireceiver_2"] = true ? "ON" : "OFF";

    size_t n = serializeJson(doc, buffer);
    
    bool published = client.publish(stateTopic.c_str(), buffer, n);
    if (published) {
      //Serial.println("Data published successfully!");
    } else {
      Serial.println("Failed to publish data.");
    }

    // Importante: Chame client.loop() para manter a conexão MQTT viva e processar callbacks
    client.loop();

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



void recebeIR() {
  // Check if the IR code has been received.
    Serial.print(results.value, HEX);

    if (results.value == 0xFFA25D) {   // 1
      Serial.println("=1");
    } 
    if (results.value == 0xFF629D) {   // 1
      Serial.println("=2");
    } 
    if (results.value == 0xFFE21D) {   // 
      Serial.println("=3");
    } 
    if (results.value == 0xFF22DD) {   // 
      doc["ireceiver_4"] = true ? "ON" : "OFF";
      Serial.println("=4");
    } 
    if (results.value == 0xFF02FD) {   //
      ireceiver_5_state = !ireceiver_5_state;
      Serial.println("=5");
    } 
    if (results.value == 0xFFC23D) {   // 
      Serial.println("=6");
    } 
    if (results.value == 0xFFE01F) {   // 
      Serial.println("=7");
    } 
    if (results.value == 0xFFA857) {   // 
      Serial.println("=8");
    } 
    if (results.value == 0xFF906F) {   // 
      Serial.println("=9");
    } 
    if (results.value == 0xFF9867) {   // 
      Serial.println("=0");
    } 
    if (results.value == 0xFFB04F) {   // 
      Serial.println("=#");
    } 
    if (results.value == 0xFF38C7) {   // 
      Serial.println("=ok");
    } 
    if (results.value == 0xFF18E7) {   // up
      Serial.println("=up");
    }
    if (results.value == 0xFF4AB5) {   // down
      Serial.println("=down");
    }        
    if (results.value == 0xFF10EF) {   // left
      Serial.println("=left");
    }    
    if (results.value == 0xFF5AA5) {   // right
      Serial.println("=right");
    }    
    if (results.value == 0xFF6897) {   // estrelas
      Serial.println("=star");
    }    
    Serial.println("\n-----------------");
    delay(100);

    doc["ireceiver_5"] = ireceiver_5_state ? "ON" : "OFF"; 
}



// Função de callback para lidar com mensagens recebidas
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida no tópico: ");
  Serial.println(topic);

  // Converte o payload em uma string
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Conteúdo da mensagem: ");
  Serial.println(message);
  Serial.println(String(topic));
  Serial.println(commandTopic);
  // Exemplo 1: Ação baseada no valor da mensagem recebida
  if (String(topic) == commandTopic) {
    if (message == "\"ACTIVATE\"") {
      Serial.println("Ação: Ativando o sistema!");
      // Adicione aqui o código para a ação de ativação
    } else if (message == "\"DEACTIVATE\"") {
      Serial.println("Ação: Desativando o sistema!");
      // Adicione aqui o código para a ação de desativação
    } else { 
      Serial.println("não entendi"); 
      Serial.println(message);
    }
  }

  // Exemplo 2: Outra ação baseada em um comando diferente
  if (message == "1") {
    Serial.println("Ação: Variáveis reiniciadas!");
  }
}

void sendMQTT_IReceiver_io_DiscoveryMsg(int indice) {
  String discoveryTopic = "homeassistant/binary_sensor/IReceiver_" + String(indice) + "/config";

  DynamicJsonDocument doc(1024);
  char buffer[256];

  // Definindo o nome do dispositivo, o tópico de estado e outros atributos do binary_sensor
  doc["name"] = "IReceiver I/O" + String(indice);
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
