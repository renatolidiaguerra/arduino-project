#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

// lolin(wemos) d1 mini clone + reinstalar drive

// Informações do WiFi
#define WIFISSID "warbrito"
#define WIFIPASS "gb240820"

#define SLEEP_TIME 120e6  // Tempo de sono em microsegundos (1 minuto)

// Informações do Servidor MQTT
const char* mqttServer = "192.168.15.10";
const int mqttPort = 1883;
const char* mqttUser = "renatobrito";
const char* mqttPass = "gb240820";

String mqttName       = "Sensor Luminosidade Sala";
String topicState     = "home/generic/lightsensor/state";   
String discoveryTopic = "homeassistant/sensor/lightsensor/config";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

const int sensorPin = A0; // Pino analógico conectado ao sensor LDR
const int intervaloLeitura = 5000; // Intervalo entre leituras em milissegundos (5 segundos)

// Piscada inicial do LED interno (3 vezes em 2 segundos)
const int ledPin = LED_BUILTIN; // Pino do LED interno (verifique a documentação da placa)

void setup() {
  Serial.begin(9600);

  Serial.println("\n\nVersao v05.4");

  // pinMode(ledPin, OUTPUT);
  // digitalWrite(ledPin, LOW); // Liga o LED

  WiFi.begin(WIFISSID, WIFIPASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(5000);
    Serial.println("Connecting to WiFi...");
  }
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

  Serial.println("Connected to WiFi");
  mqttClient.setServer(mqttServer, mqttPort);
  
  if (!mqttClient.connected()) {
    reconnect();
  }

  sendMqttDiscovery();

  pinMode(sensorPin, INPUT); // Configura o pino do sensor como entrada

  // -----------
  int valorSensor = analogRead(sensorPin); // Lê o valor do sensor LDR
  float valorfodase = 1024 - valorSensor;
  int valorfinal = (valorfodase - 190) / 810 * 100;
  // valorfinal = round(valorfinal * 100) / 100;
  // -----------

  delay(intervaloLeitura); // Aguarda o intervalo entre leituras

  DynamicJsonDocument doc(1024);
  char buffer[256];

  doc["lightsensor"] = valorfinal;

  size_t n = serializeJson(doc, buffer);
  
  bool published = mqttClient.publish(topicState.c_str(), buffer, n);
  if (published) {
    Serial.println("Data published successfully!");
    blink();
  } else {
    Serial.println("Failed to publish data.");
  }
  // Importante: Chame mqttClient.loop() para manter a conexão MQTT viva
  mqttClient.loop();

  delay(10000);
  
  Serial.println("Entrando em sleep");
  ESP.deepSleep(SLEEP_TIME);

}

void loop() { }


void reconnect() {
  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT...");
    if (mqttClient.connect("Modelo01", mqttUser, mqttPass)) {
      Serial.println("Connected to MQTT");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void sendMqttDiscovery() {

  DynamicJsonDocument doc(1024);
  char buffer[256];

  doc["name"] = mqttName;
  doc["stat_t"] = topicState;
  doc["unit_of_meas"] = "%";
  doc["dev_cla"] = "illuminance";
  doc["frc_upd"] = true;
  doc["val_tpl"] = "{{ value_json.lightsensor|default(0) }}";
  //doc["expire_after"] = 240;  // Sensor inativo após 60 segundos

  size_t n = serializeJson(doc, buffer);
  mqttClient.publish(discoveryTopic.c_str(), buffer, n);

  mqttClient.loop();
  blink();
}

void blink() {
  // for(int i = 0; i < 5; i++) {
  //   digitalWrite(ledPin, HIGH); // Desliga o LED
  //   delay(50);
  //   digitalWrite(ledPin, LOW); // Desliga o LED
  //   delay(50);
  // }
}