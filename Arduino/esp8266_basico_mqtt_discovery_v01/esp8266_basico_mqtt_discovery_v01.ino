// ------------------------------------
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

//Informações do WiFi
#define WIFISSID "warbrito"
#define WIDIPASS "gb240820"

//Informações do Servidor MQTT
const char* mqttserver = "192.168.15.10";
int mqttserverport = 1883;
const char* mqttuser = "renatobrito";
const char* mqttpass = "gb240820";

//Variáveis
WiFiClient wifiClient;
PubSubClient ConexaoMQTT(wifiClient);



const int ledPin = 2;    // the number of the LED pin
int valor_sensor = 0;


void setup() {
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);

  Serial.begin(115200);
  Serial.println("Iniciando");


  WiFi.begin(WIFISSID, WIDIPASS);
  delay(5000);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Conectado! IP address: ");
  Serial.println(WiFi.localIP());

  if (!ConexaoMQTT.connected()) {
    reconnect();
  }

  //Configura Sensores através do MQTT Discovery do Home Assistant
  String novosensor = "{\"expire_after\": \"6FD0\", \"icon\": \"mdi:gesture-tap-button\", \"name\": \"SensorSOLOv25\", \"state_topic\": \"grupo1/sensorsolo/state\"}";
  ConexaoMQTT.publish("homeassistant/sensor/grupo1/sensorsolo/config",novosensor.c_str(),false);

  // <discovelry_prefix/<component>/node_id/object_id/config
  // sensor
  // binary_sensor
  // switch
  // light
  // fan

}

void loop() {

  if (!ConexaoMQTT.connected()) {
    reconnect();
  }

    digitalWrite(ledPin, HIGH);
    ConexaoMQTT.publish("grupo1/sensorsolo/state","ON");
    delay(1000);

    ConexaoMQTT.publish("grupo1/sensorsolo/state","OFF");
    digitalWrite(ledPin, LOW);
    delay(1000);
   

    Serial.println("pisca!");

    valor_sensor += 1;
    if (valor_sensor >= 100) {
      valor_sensor = 0;
    }


  
}

// ------------------------------------
void reconnect() {
// ------------------------------------
  ConexaoMQTT.setServer(mqttserver, mqttserverport);
  while (!ConexaoMQTT.connected()) {
    Serial.println("Conectando ao Broker MQTT");
    ConexaoMQTT.connect("esp8266",mqttuser,mqttpass);
    delay(3000);
  }
  Serial.println("MQTT conectado");
}