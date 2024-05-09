#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
// #include <ping1d.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// SSD1306  -  NodeMCU (ESP8266)
// ------------------------------
// VCC      -  3.3V (ou VCC do NodeMCU)
// GND      -  GND
// SCL      -  GPIO22
// SDA      -  GPIO21

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const char* ssid = "warbrito";      // Nome da rede Wi-Fi
const char* password = "gb240820"; // Senha da rede Wi-Fi

const char* ip1 = "192.168.15.10"; // Primeiro endereço IP
const char* ip2 = "192.168.15.20"; // Segundo endereço IP

bool new_WL_CONNECTED = true;
bool HA_CONNECTED = true;

int counterHA = 0;
int counterRouter = 0;
int maxCounterRouter = 3;
int maxCounterHA = 5;
int delayDefault = 500;
int tempoInicializacao = 60;   // segundos

int i = 0;
int j = 0;

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);

  Serial.begin(115200);
  Serial.println("Sistema iniciado");

  exibirMensagemInicial();
}
// =============================================
void loop() {
// =============================================
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  
  // i = i + 1;

  // if (i >= 7) {
  //   if (i >= 9) {
  //     i = 0;
  //     HA_CONNECTED = true;   // tenta ping -> sucesso!
  //   }
  //   HA_CONNECTED = false;  // tenta ping -> erro
  // } else {
  //   HA_CONNECTED = true;   // tenta ping -> sucesso!
  // }  

  j = j + 1;

  if (j >= 7) {
    if (j >= 9) {
      j = 0;
      new_WL_CONNECTED = true;   // tenta router -> sucesso!
    }
    new_WL_CONNECTED = false;  // tenta router -> erro
  } else {
    new_WL_CONNECTED = true;   // tenta router -> sucesso!
  }

  controleTotal();
  delay(1000);
}
// =============================================
void exibirBarraDeProgresso(int tempoTotal) {
// =============================================
  
  int larguraBarra = SCREEN_WIDTH - 2;  // Largura disponível para a barra de progresso
  int alturaBarra = 13;  // Altura da barra de progresso
  int xInicial = 1;  // Posição inicial X da barra de progresso
  int yInicial = 1; //(SCREEN_HEIGHT - alturaBarra) / 2;  // Posição Y da barra de progresso
  
  int progresso = 0;  // Valor atual do progresso
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
    delay(delayDefault);
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
    delay(delayDefault);
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
    if (new_WL_CONNECTED) {
      display.print("ok");
      // display.print(j);
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
    if (!new_WL_CONNECTED) {
      return;
    }
    display.setCursor(0, 50);
    display.print("HA ");
    if (HA_CONNECTED) {
      display.print("ok");
      // display.print(i);
    } else {
      display.print(counterHA);
      display.print("/");
      display.println(maxCounterHA);
      exibirBarraDeProgresso(60);
    }
}
// =============================================
void exibirRestartHA() {
// =============================================
  int tempoTotal = 60;  // Tempo total da contagem regressiva
  
  while (tempoTotal > 0) {
    display.clearDisplay();
    
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print("boot in ");
    display.print(tempoTotal);

    exibirStatusRouter();
    
    display.setCursor(0, 50);
    display.println("HA restart!");
    
    display.display();
    tempoTotal--;
    delay(delayDefault);
  }
}
// =============================================
void exibirRestartRouter() {
// =============================================
  int tempoTotal = 60;  // Tempo total da contagem regressiva
  
  while (tempoTotal > 0) {
    display.clearDisplay();
    
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print("boot in ");
    display.print(tempoTotal);

    //exibirStatusRouter();
    
    display.setCursor(0, 25);
    display.println("Wi-Fi");
    display.println("restarting");
    
    display.display();
    tempoTotal--;
    delay(delayDefault);
  }
}
// =============================================
void checkSaudeHA() {
// =============================================
  if (!HA_CONNECTED) {
    counterHA = counterHA + 1;
  }
}
// =============================================
void checkSaudeRouter() {
// =============================================
  if (!new_WL_CONNECTED) {
    counterRouter ++;
  }
}
// =============================================
void checkRestartRouter() {
// =============================================
  if (counterRouter == maxCounterRouter) {
    counterRouter = 0;
    exibirRestartRouter(); 
  }
}
// =============================================
void checkRestartHA() {
// =============================================
  if (counterHA == maxCounterHA) {
    counterHA = 0;
    exibirRestartHA(); 
  }
}
// =============================================
void exibirMensagemInicial() {
// =============================================
  display.clearDisplay();
  
  display.setTextSize(2);
  display.setTextColor(WHITE);
  // display.setCursor(0, 0);
  // display.print("-RENATOB-");
  
  display.setCursor(0, 20);
  display.println("Iniciando");
  display.println("Sistema");
  
  display.setTextSize(1);
  display.setCursor(50, 56);
  display.println("Renatob-2023");

  display.display();
  exibirBarraDeProgresso(tempoInicializacao);
}

