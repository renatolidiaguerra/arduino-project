/*
 * This is a basic example on how to use Espalexa and its device declaration methods.
 */
#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <Espalexa.h>

// prototypes
boolean connectWifi();

int pinGRN = 2;
int pinYLW = 27;

//callback functions
void firstLightChanged(uint8_t brightness);
void secondLightChanged(uint8_t brightness);
void thirdLightChanged(uint8_t brightness);

// Change this!!
const char* ssid = "warbrito";
const char* password = "gb240820";

boolean wifiConnected = false;

Espalexa espalexa;

EspalexaDevice* device3;  //this is optional

void setup() {
  Serial.begin(115200);

  pinMode(pinGRN, OUTPUT);
  pinMode(pinYLW, OUTPUT);

  digitalWrite(pinGRN, LOW);
  digitalWrite(pinYLW, LOW);

  // Initialise wifi connection
  wifiConnected = connectWifi();

  if (wifiConnected) {

    // Define your devices here. -> aqui vai o nome do dispositivo na Alexa
    espalexa.addDevice("ESPDevice01", firstLightChanged);   //simplest definition, default state off
    espalexa.addDevice("ESPDevice02", secondLightChanged);  //third parameter is beginning state (here fully on)

    // device3 = new EspalexaDevice("Light 3", thirdLightChanged); //you can also create the Device objects yourself like here
    // espalexa.addDevice(device3); //and then add them
    // device3->setValue(128); //this allows you to e.g. update their state value at any time!

    espalexa.begin();

  } else {
    while (1) {
      Serial.println("Cannot connect to WiFi. Please check data and reset the ESP.");
      delay(2500);
    }
  }
}

void loop() {
  espalexa.loop();
  delay(1);
}

//our callback functions
void firstLightChanged(uint8_t brightness) {
  Serial.print("Device 1 changed to ");

  //do what you need to do here

  //EXAMPLE
  if (brightness) {
    Serial.print("ON, brightness ");
    Serial.println(brightness);

    digitalWrite(pinGRN, HIGH);
    digitalWrite(pinYLW, LOW);
  } else {
    Serial.println("OFF");
    digitalWrite(pinGRN, LOW);
  }
}

void secondLightChanged(uint8_t brightness) {
  Serial.print("Device 2 changed to ");

  //do what you need to do here

  //EXAMPLE
  if (brightness) {
    Serial.print("ON, brightness ");
    Serial.println(brightness);

    digitalWrite(pinYLW, HIGH);
  } else {
    Serial.println("OFF");
    digitalWrite(pinYLW, LOW);
  }
}

void thirdLightChanged(uint8_t brightness) {
  //do what you need to do here
}

// connect to wifi – returns true if successful or false if not
boolean connectWifi() {
  boolean state = true;
  int i = 0;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 20) {
      state = false;
      break;
    }
    i++;
  }
  Serial.println("");
  if (state) {
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Connection failed.");
  }
  return state;
}
