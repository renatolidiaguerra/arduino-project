#include <PubSubClient.h>
#include <ESP8266WiFi.h>

// Informações do WiFi
#define WIFISSID "warbrito"
#define WIFIPASS "gb240820"

#define SLEEP_TIME 90e6  // Tempo de sono em microsegundos (1 minuto)

// Informações do Servidor MQTT
const char* mqttServer = "192.168.15.10";
const int mqttPort = 1883;
const char* mqttUser = "renatobrito";
const char* mqttPass = "gb240820";
const char* topicControl = "homeassistant/esp32/sensorluz/control";
const char* topicState   = "homeassistant/esp32/sensorluz/state";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);


const int sensorPin = A0; // Pino analógico conectado ao sensor LDR
const int intervaloLeitura = 5000; // Intervalo entre leituras em milissegundos (5 segundos)

// Piscada inicial do LED interno (3 vezes em 2 segundos)
const int ledPin = LED_BUILTIN; // Pino do LED interno (verifique a documentação da placa)

void setup() {
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  for (int i = 0; i < 3; i++) {
    digitalWrite(ledPin, HIGH); // Liga o LED
    delay(500); // Aguarda 500ms
    digitalWrite(ledPin, LOW); // Desliga o LED
    delay(500); // Aguarda 500ms
  }

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
  String payloadConfig = "{\"name\": \"SensorLuz\", \"command_topic\": \"homeassistant/esp32/sensorluz/control\", \"state_topic\": \"homeassistant/esp32/modesensorluzlo/state\"}";
  mqttClient.publish("homeassistant/esp32/sensorluz/config", payloadConfig.c_str(), true);

  pinMode(sensorPin, INPUT); // Configura o pino do sensor como entrada
}

void loop() {

  // -----------
  int valorSensor = analogRead(sensorPin); // Lê o valor do sensor LDR
  float valorfodase = 1024 - valorSensor;
  float valorfinal = (valorfodase - 190) / 810 * 100;
  // -----------

  delay(intervaloLeitura); // Aguarda o intervalo entre leituras

  char msg[50];
  dtostrf(valorfinal, 6, 2, msg); // Converte float para string

  mqttClient.publish(topicState, msg);

  delay(10000);
  Serial.println("Entrando em sleep");
  ESP.deepSleep(SLEEP_TIME);

}


void reconnect() {
  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT...");
    if (mqttClient.connect("Modelo01", mqttUser, mqttPass)) {
      Serial.println("Connected to MQTT");
//      mqttClient.publish(topicState, "online");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
      // contador_restart += 1;
      // if (contador_restart > 12) {
      //   esp_restart();
      // }
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {

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
