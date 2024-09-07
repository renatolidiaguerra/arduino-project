#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#include <ArduinoJson.h>

#define DHTPIN D5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const char *WIFI_SSID = "warbrito";
const char *WIFI_PASSWORD = "gb240820";

const char* mqttServer = "192.168.15.10"; // The IP of your MQTT broker
const int mqttPort = 1883;
const char* mqttUser = "renatobrito";
const char* mqttPassword = "gb240820";

// Define some variables we need later
float humidity = 4;
float temperature = 7;

int sensorPin = A0; 
int moisture = 0;

int moisture_low = 650;
int moisture_high = 350;
int moisturePercentage = 0;

// My own numerical system for registering devices.
int sensorNumber = 3;
String mqttName = "Plant sensor " + String(sensorNumber);
String stateTopic = "home/plants/" + String(sensorNumber) + "/state";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void setup() {
  Serial.begin(9600);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Versão v01");
  Serial.println("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

  Serial.println("Connected to Wi-Fi");

  client.setServer(mqttServer, mqttPort);

  Serial.println("Connecting to MQTT");

  while (!client.connected()) {
    Serial.print(".");

    if (client.connect(mqttName.c_str(), mqttUser, mqttPassword)) {

      Serial.println("Connected to MQTT");

      sendMQTTTemperatureDiscoveryMsg();  // !!!
      sendMQTTHumidityDiscoveryMsg();     // !!
      sendMQTTMoistureDiscoveryMsg();
      // sendMQTTMoisture2DiscoveryMsg();
    } else {

      Serial.println("failed with state ");
      Serial.print(client.state());
      delay(2000);

    }
  }

  dht.begin();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("===== Sending Data =====");

    humidity = dht.readHumidity();
    temperature = dht.readTemperature();
    moisture = analogRead(sensorPin);

    if (isnan(humidity)) {
      humidity = 0;
    }

    if (isnan(temperature)) {
      temperature = 0;
    }

    // Map moisture sensor values to a percentage value
    moisturePercentage = map(moisture, moisture_low, moisture_high, 0, 100);

    DynamicJsonDocument doc(1024);
    char buffer[256];

    doc["humidity"] = 11; //humidity;
    doc["temperature"]   = 12;  //temperature;
    doc["moisture"] = 13;
    // doc["moisture2"] = 22;

    size_t n = serializeJson(doc, buffer);

    bool published = client.publish(stateTopic.c_str(), buffer, n);

    // Print the sensor values to Serial out (for debugging)
    Serial.println("published! ");
    // Serial.println(published);
    // Serial.println("humidity: ");
    // Serial.println(humidity);
    // Serial.println("temperature: ");
    // Serial.println(temperature);
    // Serial.println("moisture %: ");
    // Serial.println(moisturePercentage);
  }
  else {
    Serial.println("WiFi Disconnected");
  }

  // Go into deep sleep mode for 60 seconds
  Serial.println("Deep sleep mode for 60 seconds");
  ESP.deepSleep(10e6);
}

void sendMQTTTemperatureDiscoveryMsg() {
  String discoveryTopic = "homeassistant/sensor/plant_sensor_" + String(sensorNumber) + "/temperature/config";

  DynamicJsonDocument doc(1024);
  char buffer[256];

  doc["name"] = "Plant " + String(sensorNumber) + " Temperature";
  doc["stat_t"]   = stateTopic;
  doc["unit_of_meas"] = "°C";
  doc["dev_cla"] = "temperature";
  doc["frc_upd"] = true;
  doc["val_tpl"] = "{{ value_json.temperature|default(0) }}";

  size_t n = serializeJson(doc, buffer);
  client.publish(discoveryTopic.c_str(), buffer, n);
  Serial.println("Publicando Temperatura");
}

void sendMQTTHumidityDiscoveryMsg() {
  String discoveryTopic = "homeassistant/sensor/plant_sensor_" + String(sensorNumber) + "/humidity/config";

  DynamicJsonDocument doc(1024);
  char buffer[256];

  doc["name"] = "Plant " + String(sensorNumber) + " Humidity";
  doc["stat_t"]   = stateTopic;
  doc["unit_of_meas"] = "%";
  doc["dev_cla"] = "humidity";
  doc["frc_upd"] = true;
  doc["val_tpl"] = "{{ value_json.humidity|default(0) }}";

  size_t n = serializeJson(doc, buffer);

  client.publish(discoveryTopic.c_str(), buffer, n);
  Serial.println("Publicando Umidade");
}

// ===============================================
void sendMQTTMoisture1DiscoveryMsg() {
  String discoveryTopic = "homeassistant/sensor/plant_sensor_" + String(sensorNumber) + "/moisture1/config";

  DynamicJsonDocument doc(1024);
  char buffer[256];

  doc["name"] = "Plant " + String(sensorNumber) + " Moisture1";
  doc["stat_t"]   = stateTopic;
  doc["frc_upd"] = true;
  doc["val_tpl"] = "{{ value_json.moisture1|default(0) }}";

  size_t n = serializeJson(doc, buffer);

  client.publish(discoveryTopic.c_str(), buffer, n);
  Serial.println("Publicando Moisture1");
}


// ===============================================
void sendMQTTMoisture2DiscoveryMsg() {
  String discoveryTopic = "homeassistant/sensor/plant_sensor_" + String(sensorNumber) + "/moisture2/config";

  DynamicJsonDocument doc(1024);
  char buffer[256];

  doc["name"] = "Plant " + String(sensorNumber) + " Moisture2";
  doc["stat_t"]   = stateTopic;
  doc["frc_upd"] = true;
  doc["val_tpl"] = "{{ value_json.moisture2|default(0) }}";

  size_t n = serializeJson(doc, buffer);

  client.publish(discoveryTopic.c_str(), buffer, n);

  Serial.println("Publicando Moisture2");
}


// ===============================================
void sendMQTTMoistureDiscoveryMsg() {
  String discoveryTopic = "homeassistant/sensor/plant_sensor_" + String(sensorNumber) + "/moisture/config";

  DynamicJsonDocument doc(1024);
  char buffer[256];

  doc["name"] = "Plant " + String(sensorNumber) + " Moisture";
  doc["stat_t"]   = stateTopic;
  doc["frc_upd"] = true;
  doc["val_tpl"] = "{{ value_json.moisture|default(0) }}";

  size_t n = serializeJson(doc, buffer);

  client.publish(discoveryTopic.c_str(), buffer, n);

  Serial.println("Publicando Moisture");
}


void loop() {}