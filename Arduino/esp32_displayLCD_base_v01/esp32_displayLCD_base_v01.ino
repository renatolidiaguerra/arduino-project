/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

// lolin wemos d1 r1
// esp32 c3 super mini



#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define I2C_SDA 4
#define I2C_SCL 5

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(115200);

  Wire.begin(I2C_SDA, I2C_SCL);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
}

void loop() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);

  display.setCursor(0, 25);
  display.print("Wi-Fi-");
  display.print("ok");
 
  display.setCursor(0, 50);
  display.print("1234567890");
  //display.print("ok/nok");

  display.display(); 

  exibirBarraDeProgresso(20);

  delay(2000);

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
    //delay_function(delayDefault);
    delay(100);
  }
}