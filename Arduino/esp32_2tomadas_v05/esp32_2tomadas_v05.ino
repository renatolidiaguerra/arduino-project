#include <WiFi.h>
#include <ESP32Ping.h> /*including the ping library*/

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

int pinMaster = 12;  /// trocar para releMaster e releSlave  +  pensar em led externo
int pinSlave = 27;

// int pinLed = 2;
int pinButton = 5;
int buttonState = 0;

// int boardLed = 2;

bool HA_CONNECTED = false;
bool Router_CONNECTED = false;
bool on = true;
bool off = false;

int counterHA = 0;
int counterRouter = 0;
int maxCounterRouter = 9;
int maxCounterHA = 5;
int delayDefault = 1000;      // em ms: 20=debug; 1000=default
int tempoInicializacao = 60;  // em segundos
int intervaloCheck = 120000;  // tempo entre verificações no sistema (principalmente por conta do ping)

// int i = 0;
// int j = 0;

// =============================================
void setup() {
// =============================================

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);

  Serial.begin(115200); /*Baud rate for serial communication*/
  pinMode(pinSlave, OUTPUT);
  pinMode(pinMaster, OUTPUT);

  // pinMode(pinLed, OUTPUT);
  pinMode(pinButton, INPUT);

  // exibir_inicio_conexao();

  // digitalWrite(pinSlave, HIGH);   // low = on;  high=off
  alterarStatusPlugueSlave(on);
  // digitalWrite(pinMaster, HIGH);  // low = on;  high=off
  alterarStatusPlugueMaster(on);

  // digitalWrite(pinLed, HIGH);

  // digitalWrite(boardLed, LOW);

  Serial.println("\n\n\n\n\n\n\n\n\n\n");
  Serial.println("Starting system...");

  WiFi.begin(ssid, password); /*Begin WiFi connection*/

  exibirMensagemInicial();
}

// =============================================
void loop() {
// =============================================
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);

  controleTotal();
  //delay_function(1000);
}
// // =============================================
// void old_loop() {
// // =============================================
//   // ver necessidade disso
//   display.clearDisplay();
//   display.setTextColor(WHITE);
//   display.setTextSize(2);

//   WiFi.begin(ssid, password); /*Begin WiFi connection*/

//   while (WiFi.status() != WL_CONNECTED) {

//     display.setCursor(0, 0);
//     display.println("Waiting WIFI");

//     counterRouter += 1;
//     Serial.print("Connecting to WiFi... - attempt ");
//     Serial.print(counterRouter);
//     Serial.println(" of 5");

//     // delay_function de 1 minuto
//     for (int i = 0; i < 120; i++) {
//       delay_function(20);
//       digitalWrite(pinLed, HIGH);
//       delay_function(500);
//       digitalWrite(pinLed, LOW);
//       if (WiFi.status() == WL_CONNECTED) { break; }
//     }

//     if (WiFi.status() != WL_CONNECTED) {
//       Serial.print("Reconnecting...");
//       WiFi.disconnect();
//       WiFi.reconnect();
//     }

//     if (counterRouter >= 5) {  // fora durante 5 minutos
//       Serial.println("Conection Router failed.");
//       Serial.println("Shut down...");
//       digitalWrite(pinLed, HIGH);
//       digitalWrite(pinMaster, LOW);
//       delay_function(1 * 60000);  // 1 minuto
//       digitalWrite(pinMaster, HIGH);
//       digitalWrite(pinLed, LOW);
//       Serial.println("Starting Router...");
//       WiFi.disconnect();
//       WiFi.reconnect();
//       delay_function(5 * 60000);  // 5 minutos
//       counterRouter = 0;
//     }
//   }

//   Serial.println("Router online!");
//   counterRouter = 0;

//   while (WiFi.status() == WL_CONNECTED) {

//     digitalWrite(pinLed, HIGH);

//     bool success = Ping.ping("192.168.15.10", 3);  // homeassistant.local:8123       Ping.ping("192.168.15.10", 3);

//     if (success) {
//       display.setCursor(0, 20);
//       display.print("Ping ok!");

//       Serial.println("Ping successful");
//       counterHA = 0;
//     } else {

//       counterHA += 1;

//       display.setCursor(0, 20);
//       display.println("Ping failed");
//       display.print(counterHA);
//       display.println(" of 5");

//       Serial.print("Ping failed... - attempt ");
//       Serial.print(counterHA);
//       Serial.println(" of 5");
//     }

//     // delay_function de 1 minuto
//     for (int i = 0; i < 6; i++) {
//       if (counterHA > 0) {
//         for (int x = 0; x < 10; x++) {
//           digitalWrite(pinLed, LOW);
//           delay_function(1000);
//           digitalWrite(pinLed, HIGH);
//           delay_function(10);
//         }
//       } else {
//         digitalWrite(pinLed, LOW);
//         delay_function(10000);
//         digitalWrite(pinLed, HIGH);
//         delay_function(10);
//       }
//     }

//     if (counterHA >= 5) {  // fora durante 5 minutos
//       Serial.println("Conection Home Assistant failed.");
//       Serial.println("Shut down...");
//       digitalWrite(pinLed, HIGH);
//       digitalWrite(pinSlave, LOW);
//       delay_function(1 * 60000);  // 1 minuyo
//       digitalWrite(pinSlave, HIGH);
//       Serial.println("Starting Home Assistant...");
//       delay_function(5 * 60000);  // 5 minutos para restart do sistema
//       Serial.println("Starting ping...");
//       digitalWrite(pinLed, LOW);
//       counterHA = 0;
//     }
//   }
// }
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
  int yInicial = 1;                     //(SCREEN_HEIGHT - alturaBarra) / 2;  // Posição Y da barra de progresso

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
    //display.clearDisplay();

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
  int tempoTotal = 60;  // Tempo total da contagem regressiva
  alterarStatusPlugueSlave(off);    // desliga tomada HA
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
  alterarStatusPlugueSlave(on);    // liga tomada HA
  delay_function(1000);
}
// =============================================
void acionarRestartRouter() {
// =============================================
  int tempoTotal = 60;  // Tempo total da contagem regressiva
  alterarStatusPlugueMaster(off);    // desliga saida
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
  alterarStatusPlugueMaster(on);     // religa router
  // delay_function(1000);
  WiFi.begin(ssid, password); /*Begin WiFi connection*/
  display.setCursor(0, 25);
  display.print("Wi-Fi");
  display.println("begin");
  display.display();
}
// =============================================
void checkSaudeHA() {
// =============================================
  if (Router_CONNECTED) {

    display.clearDisplay();

    display.setCursor(0, 0);
    display.print("_");

    display.setCursor(0, 25);
    display.println("Wi-Fi ok");

    display.setCursor(0, 50);
    display.print("HA ");

    display.display();

    // digitalWrite(boardLed, HIGH);

    HA_CONNECTED = Ping.ping("192.168.15.10", 3);
    if (!HA_CONNECTED) {
      counterHA = counterHA + 1;
    } 
    // digitalWrite(boardLed, LOW);
  }
}
// =============================================
void checkSaudeRouter() {
// =============================================
  if (WiFi.status() != WL_CONNECTED) {
    Router_CONNECTED = false;
    counterRouter ++;
    Serial.print("Tentativa ");
    Serial.println(counterRouter);
    Serial.println(WiFi.status());
    // WiFi.disconnect();
    // WiFi.reconnect();
    // digitalWrite(boardLed, HIGH);
    WiFi.begin(ssid, password); 
    // digitalWrite(boardLed, LOW);
    Serial.println(WiFi.status());
  } else {
    Serial.println("Conectado");
    Serial.println(WiFi.status());
    Router_CONNECTED = true;
  }
}
// =============================================
void checkRestartRouter() {
// =============================================
  if (counterRouter == maxCounterRouter) {
    counterRouter = 0;
    acionarRestartRouter();
  }
}
// =============================================
void checkRestartHA() {
// =============================================
  if (counterHA == maxCounterHA) {
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
// =============================================
// =============================================
// =============================================

