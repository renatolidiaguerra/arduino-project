/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

// lolin wemos d1 r1 (old)
// esp32 c3 super mini ou 
//  esp32c3 dev module

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define SOIL_MOISTURE_SENSOR_PIN A0  // Pino conectado ao A0 do sensor

#define I2C_SDA 4
#define I2C_SCL 5
//      in=3.3v

int   valor_minimo = 0;     // 1325;
int   valor_maximo = 4095;
int   valor_final  = 0;

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(115200);

  Wire.begin(I2C_SDA, I2C_SCL);

  pinMode(SOIL_MOISTURE_SENSOR_PIN, INPUT);  // Define o pino do sensor como entrada

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
  int sensorValue = analogRead(SOIL_MOISTURE_SENSOR_PIN);  // Lê o valor do sensor de umidade de solo
  display.setCursor(0, 25);
  display.print("  Umidade");
  // display.print(sensorValue);
 
  // Serial.print("sensorValue: ");
  // Serial.println(sensorValue);
  
  // valor_final = ( (1 - (sensorValue - valor_minimo) / (valor_maximo - valor_minimo))) * 100;

  valor_final = 100 - ( (sensorValue - valor_minimo) * 100 / (valor_maximo - valor_minimo) );

  display.setCursor(0, 50);
  display.print("    ");
  display.print(valor_final);
  display.print("%");

  //display.display(); 

  exibirBarraDeProgresso(valor_final);


  // Serial.print("Soil Moisture Value: ");
  // Serial.println(sensorValue);  // Exibe o valor lido no monitor serial
  // Serial.print("percentual: ");
  // Serial.println(valor_final);
  // Adicione um atraso para não sobrecarregar a saída serial
  delay(500);  // Aguarda 1 segundo antes de ler novamente

}

// =============================================
void exibirBarraDeProgresso(int percentual) {
  // =============================================

  int larguraBarra = SCREEN_WIDTH - 2;  // Largura disponível para a barra de progresso
  int alturaBarra = 13;                 // Altura da barra de progresso
  int xInicial = 1;                     // Posição inicial X da barra de progresso
  int yInicial = 1;                     //(SCREEN_HEIGHT - alturaBarra) / 2;// Posição Y da barra de progresso

  int larguraAtual = 0;  // Largura atual da barra de progresso

  // Cálculo da largura atual da barra de progresso
  larguraAtual = map(percentual, 0, 100, 0, larguraBarra);

  // Limpar a área onde a barra será desenhada (caso necessário)
  display.fillRect(xInicial, yInicial, larguraBarra, alturaBarra, BLACK);

  // Desenhar a barra de progresso
  display.drawRect(xInicial, yInicial, larguraBarra, alturaBarra, WHITE);
  display.fillRect(xInicial, yInicial, larguraAtual, alturaBarra, WHITE);

  display.display();
}
