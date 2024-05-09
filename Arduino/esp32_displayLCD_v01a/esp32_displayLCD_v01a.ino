#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64


//  conexão:
//  SDA: pino 21
//  SCL: pino 22

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); // Pinos SDA e SCL

int i = 0;

void setup() {
  Serial.begin(115200);
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Falha ao iniciar o display SSD1306"));
    for (;;) ;
  }

  display.setTextSize(2);
  display.setTextColor(WHITE);
}

void loop() {
  Serial.println("Rodando 2");
  display.clearDisplay();

  // Obtenha o tamanho da mensagem "OK"
  int16_t x, y;
  uint16_t w, h;
  display.getTextBounds("OK", 0, 0, &x, &y, &w, &h);

  // Centralize a mensagem no display
  int16_t xPos = (SCREEN_WIDTH - w) / 2;
  int16_t yPos = (SCREEN_HEIGHT - h) / 2;

  display.setCursor(xPos, yPos);
  display.print("OK");
  display.print(i);
  i++;
  display.display();
}
