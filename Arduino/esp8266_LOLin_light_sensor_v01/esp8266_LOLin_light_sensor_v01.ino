#include <ESP8266WiFi.h>

const int sensorPin = A0; // Pino analógico conectado ao sensor LDR
const int intervaloLeitura = 1000; // Intervalo entre leituras em milissegundos (5 segundos)

void setup() {
  Serial.begin(115200); // Inicia a comunicação serial
  pinMode(sensorPin, INPUT); // Configura o pino do sensor como entrada
}

void loop() {
  int valorSensor = analogRead(sensorPin); // Lê o valor do sensor LDR
  //float voltagem = valorSensor * (5.0 / 1023.0); // Converte o valor para voltagem   -- resistor de 10k
  float voltagem = valorSensor * (5.0 / 1023.0) * (10000.0 / 122.0);                 // resistor de 122
  float potenciaLuz = 100.0 * (voltagem / 5.0); // Calcula a potência da luz (em %)

  Serial.print("Valor LDR: ");
  Serial.print(valorSensor);
  Serial.print(" | Voltagem: ");
  Serial.print(voltagem);
  Serial.print(" V | Potência da Luz: ");
  Serial.print(potenciaLuz);
  Serial.println(" %");

  delay(intervaloLeitura); // Aguarda o intervalo entre leituras
}
