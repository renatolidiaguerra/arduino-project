#include <WiFi.h>
//#include <ESP32Ping.h> /*including the ping library*/
#include <PubSubClient.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// SSD1306  -  NodeMCU (ESP8266)
// ------------------------------
// VCC      -  3.3V (ou VCC do NodeMCU)
// GND      -  GND
// SCL      -  GPIO22
// SDA      -  GPIO21

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const char* ssid = "warbrito";     /*Define network SSID*/
const char* password = "gb240820"; /*Define Network Password*/

// Informações do Servidor MQTT
const char* mqttServer = "192.168.15.10";
const int mqttPort = 1883;
const char* mqttUser = "renatobrito";
const char* mqttPass = "gb240820";
const char* topicControl = "nodemcu/smartplug/control";
const char* topicState = "nodemcu/smartplug/state";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);


int pinMaster = 12;  /// trocar para releMaster e releSlave  +  pensar em led externo
int pinSlave = 27;

int pinButton = 5;
int buttonState = 0;

bool HA_CONNECTED = false;
bool Router_CONNECTED = false;
bool on = true;
bool off = false;

int counterHA = 0;
int counterRouter = 0;

int maxCounterRouter = 9;       // tentativas de reconexão antes do restart fisico 
int maxCounterHA = 5;           // tentativas de reconexão antes do restart fisico  (nivel 3)

int contHAError = 0;
int maxContHAError = 5;         // tentativas de reconexão antes de acumular 1 tentativa para reconexão física (nivel 2)

int contRestartMQTT = 0;
int maxContRestartMQTT = 5;     // quantidade de testes positivos para restartar mqtt connection (evita falso positivo / bug desconectado porém status=true) nivel 1
int intervaloRestart = 5000;    // tempo de espera para a tentativa de reconexão

int delayDefault = 1000;      // em ms: 20=debug; 1000=default
int tempoInicializacao = 60;  // em segundos :60
int intervaloCheck = 120000;  // tempo entre verificações no sistema (principalmente por conta do ping) : 120000

bool debug = false;

// =============================================
void setup() {
  // =============================================

  // parametros para debut
  if (debug) {
    delayDefault = 100;     // em ms: 20=debug; 1000=default
    intervaloCheck = 2000;  // tempo entre verificações no sistema (principalmente por conta do ping) : 120000
  }

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);

  Serial.begin(115200); /*Baud rate for serial communication*/
  pinMode(pinSlave, OUTPUT);
  pinMode(pinMaster, OUTPUT);

  pinMode(pinButton, INPUT);

  alterarStatusPlugueSlave(on);
  alterarStatusPlugueMaster(on);

  Serial.println("\n\n\n\n\n\n\n\n\n\n");
  Serial.println("Starting system...");

  exibirMensagemInicial();

  WiFi.begin(ssid, password); /*Begin WiFi connection*/

  // -------------- bloco inteiro mqtt
  mqttClient.setServer(mqttServer, mqttPort);  // configura mqtt server e callback
  mqttClient.setCallback(callback);

  if (!mqttClient.connected()) {
    reconnect();
  }

  mqttClient.subscribe(topicControl);  // Subscrever ao tópico para receber comandos
                                       //Configura Sensores através do MQTT Discovery do Home Assistant
  String buttonConfig = "{\"expire_after\": \"120000\", \"name\": \"SmartPlug\", \"command_topic\": \"nodemcu/smartplug/control\", \"state_topic\": \"nodemcu/smartplug/state\"}";
  mqttClient.publish("homeassistant/switch/nodemcu/smartplug/config", buttonConfig.c_str(), false);
  // -------------- fim bloco
}

// =============================================
void loop() {
  // =============================================
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);

  controleTotal();
}

// =============================================
void delay_function(int tempo) {
  // =============================================
  for (int i = 0; i <= (tempo / 10); i++) {
    buttonState = digitalRead(pinButton);
    if (buttonState == HIGH) {
      Serial.println("Botao pressionado...restartando");

      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.println("_ATENCAO_");
      display.println(" bypass");
      display.println("  mode");
      display.display();
      delay(500);
      buttonState = 0;
      while (true) {
        buttonState = digitalRead(pinButton);
        if (buttonState == HIGH) {
          display.clearDisplay();
          display.setTextSize(2);
          display.setTextColor(WHITE);
          display.setCursor(0, 0);
          display.println("_ATENCAO_");
          display.println("");
          display.println("restarting");
          display.display();
          delay(2000);
          ESP.restart();
        }
      }
    }
    delay(10);
  }
}
// =============================================
void controleTotal() {
  // =============================================

  // mqttClient.loop();// Manter a conexão MQTT ativa

  checkSaudeRouter();
  checkSaudeHA();

  exibirStatus();

  checkRestartRouter();
  checkRestartHA();

  delayEntreCheck();
}
// =============================================
void exibirBarraDeProgresso(int tempoTotal) {
  // =============================================

  int larguraBarra = SCREEN_WIDTH - 2;  // Largura disponível para a barra de progresso
  int alturaBarra = 13;                 // Altura da barra de progresso
  int xInicial = 1;                     // Posição inicial X da barra de progresso
  int yInicial = 1;                     //(SCREEN_HEIGHT - alturaBarra) / 2;// Posição Y da barra de progresso

  int progresso = 0;     // Valor atual do progresso
  int larguraAtual = 0;  // Largura atual da barra de progresso

  while (progresso <= tempoTotal) {
    // Cálculo da largura atual da barra de progresso
    larguraAtual = map(progresso, 0, tempoTotal, 0, larguraBarra);

    // Desenhar a barra de progresso
    display.drawRect(xInicial, yInicial, larguraBarra, alturaBarra, WHITE);
    display.fillRect(xInicial, yInicial, larguraAtual, alturaBarra, WHITE);

    display.display();

    // Incrementar o progresso
    progresso++;

    // Aguardar 1 segundo
    delay_function(delayDefault);
  }
}
// =============================================
void exibirContagemRegressiva() {
  // =============================================
  display.clearDisplay();

  int tempoTotal = 60;  // Tempo total da contagem regressiva

  while (tempoTotal > 0) {

    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print("atencao:");
    display.print(tempoTotal);

    display.display();

    // Decrementar o tempo total
    tempoTotal--;

    // Aguardar 1 segundo
    delay_function(delayDefault);
  }
}
// =============================================
void exibirStatus() {
  // =============================================
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);

  exibirStatusRouter();
  exibirStatusHA();

  display.display();
}
// =============================================
void exibirStatusRouter() {
  // =============================================
  display.setCursor(0, 25);
  display.print("Wi-Fi ");
  if (Router_CONNECTED) {
    display.print("ok");
  } else {
    display.print(counterRouter);
    display.print("/");
    display.print(maxCounterRouter);
    exibirBarraDeProgresso(60);
  }
}
// =============================================
void exibirStatusHA() {
  // =============================================
  if (!Router_CONNECTED) {
    return;
  }

  display.setCursor(0, 50);
  display.print("HA ");
  if (HA_CONNECTED) {
    display.print("ok");
  } else {
    display.print(counterHA);
    display.print("/");
    display.println(maxCounterHA);
    exibirBarraDeProgresso(60);
  }
}
// =============================================
void acionarRestartHA() {
  // =============================================
  int tempoTotal = 60;            // Tempo total da contagem regressiva
  alterarStatusPlugueSlave(off);  // desliga tomada HA
  while (tempoTotal >= 0) {
    display.clearDisplay();

    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    if (tempoTotal == 0) {
      display.print("Turn on...");
    } else {
      display.print("boot in ");
      display.print(tempoTotal);
    }

    exibirStatusRouter();

    display.setCursor(0, 50);
    display.println("HA restart");

    display.display();
    tempoTotal--;
    delay_function(delayDefault);
  }
  alterarStatusPlugueSlave(on);  // liga tomada HA
  delay_function(1000);
}
// =============================================
void acionarRestartRouter() {
  // =============================================
  int tempoTotal = 60;             // Tempo total da contagem regressiva
  alterarStatusPlugueMaster(off);  // desliga saida
  while (tempoTotal >= 0) {
    display.clearDisplay();

    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);

    if (tempoTotal == 0) {
      display.print("Turn on...");
    } else {
      display.print("boot in ");
      display.print(tempoTotal);
      display.setCursor(0, 25);
      display.println("Wi-Fi");
      display.println("restarting");
    }

    display.display();
    tempoTotal--;
    delay_function(delayDefault);
  }
  alterarStatusPlugueMaster(on);  // religa router

  WiFi.begin(ssid, password); /*Begin WiFi connection*/
  display.setCursor(0, 25);
  display.print("Wi-Fi");
  display.println("begin");
  display.display();
}
// =============================================
void checkSaudeRouter() {
  // =============================================
  if (WiFi.status() != WL_CONNECTED) {
    Router_CONNECTED = false;
    counterRouter++;
    WiFi.begin(ssid, password);
  } else {
    Router_CONNECTED = true;
  }
}
// =============================================
void checkSaudeHA() {
  // =============================================
  if (Router_CONNECTED) {

    display.clearDisplay();

    display.setCursor(0, 0);
    display.print(" _");

    display.setCursor(0, 25);
    display.println("Wi-Fi ok");

    display.setCursor(0, 50);
    display.print("HA    ");

    display.display();

    HA_CONNECTED = mqttClient.connected();

    contRestartMQTT++;
    if (contRestartMQTT >= maxContRestartMQTT) {
      contRestartMQTT = 0;
      Serial.println("Disconnecting...");
      mqttClient.disconnect();
      HA_CONNECTED = false;
    }

    if (!HA_CONNECTED) {
      reconnect();
    }
  }
}
// =============================================
void checkRestartRouter() {
  // =============================================
  if (counterRouter >= maxCounterRouter) {
    counterRouter = 0;
    acionarRestartRouter();
  }
}
// =============================================
void checkRestartHA() {
  // =============================================
  if (counterHA >= maxCounterHA) {
    counterHA = 0;
    acionarRestartHA();
  }
}
// =============================================
void exibirMensagemInicial() {
  // =============================================
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(WHITE);

  display.setCursor(0, 20);
  display.println("Iniciando");
  display.println("Sistema");

  display.setTextSize(1);
  display.setCursor(50, 56);
  display.println("Renatob-2023");

  display.display();
  exibirBarraDeProgresso(tempoInicializacao);
}
// =============================================
void alterarStatusPlugueMaster(bool ligado) {
  // =============================================
  if (ligado) {
    digitalWrite(pinMaster, HIGH);
  } else {
    digitalWrite(pinMaster, LOW);
  }
}
// =============================================
void alterarStatusPlugueSlave(bool ligado) {
  // =============================================
  if (ligado) {
    digitalWrite(pinSlave, HIGH);
  } else {
    digitalWrite(pinSlave, LOW);
  }
}
// =============================================
void delayEntreCheck() {
  // =============================================
  if (Router_CONNECTED && HA_CONNECTED) {
    delay_function(intervaloCheck);
  }
}
// =============================================
void reconnect() {
// =============================================
  contHAError = 0;
  while (!mqttClient.connected() && contHAError <= maxContHAError) {
    Serial.println("Connecting to MQTT...");
    if (mqttClient.connect("SmartPlug", mqttUser, mqttPass)) {
      Serial.println("Connected to MQTT");
      HA_CONNECTED = true;
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Retrying...");
      delay_function(intervaloRestart);
      HA_CONNECTED = false;
      contHAError++;
    }
  }
  if (contHAError >= maxContHAError) {
    counterHA = counterHA + 1;
  }
}
// =============================================
void callback(char* topic, byte* payload, unsigned int length) {
  // =============================================
  Serial.print("Received message [");
  Serial.print(topic);
  Serial.print("]: ");

  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

  Serial.println();
}

// =============================================
// =============================================
// =============================================
// =============================================
