#include <ESP8266WiFi.h>

const int sensorPin = A0; // Pino analógico conectado ao sensor LDR
const int intervaloLeitura = 100; // Intervalo entre leituras em milissegundos (5 segundos)

void setup() {
  Serial.begin(115200); // Inicia a comunicação serial
  pinMode(sensorPin, INPUT); // Configura o pino do sensor como entrada
}

void loop() {
   int valorSensor = analogRead(sensorPin); // Lê o valor do sensor LDR
  float voltagem = valorSensor * (5 / 1023.0); // Converte o valor para voltagem
  float correnteLuz = voltagem / 10000.0; // Calcula a corrente da luz (em Amperes)
  float potenciaLuz = correnteLuz * voltagem; // Calcula a potência da luz (em Watts)

  // limite inferior: 190
  // limite superior: 1000
  // diferença: 810

  // 1024 - x = 900   ou:   1024-124=900

  float valorfodase = 1024 - valorSensor;

  float valorfinal = (valorfodase - 190) / 810 * 100;
  //float valorfinal = (810 - valorSensor) / 810 * 100;
  
  Serial.print("Valor LDR: ");
  Serial.print(voltagem);
  Serial.print(" V |  Corrente da Luz: ");
  Serial.print(valorSensor);
  Serial.print("   |  ValorFodase: ");
  Serial.print(valorfodase);
  Serial.print("   |  Valor Final: ");
  Serial.println(valorfinal);
  // Serial.print(" | Voltagem: ");
  // Serial.print(correnteLuz);
  // Serial.print(" A | Potência da Luz: ");
  // Serial.print(potenciaLuz);
  // Serial.println(" W");

  delay(intervaloLeitura); // Aguarda o intervalo entre leituras

}
