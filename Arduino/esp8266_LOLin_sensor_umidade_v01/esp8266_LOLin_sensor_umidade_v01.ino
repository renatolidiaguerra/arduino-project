const int sensorPin = A0; // Pino do sensor de umidade de solo conectado ao A0 do NodeMCU

void setup() {
  Serial.begin(115200);
}

void loop() {
  // Leia o valor analógico do sensor de umidade de solo
  int sensorValue = analogRead(sensorPin);

  // Converta o valor analógico para um valor de umidade em porcentagem (0% a 100%)
  float humidity = 100 - map(sensorValue, 400, 1023, 0, 100);

  // Exiba o valor da umidade no monitor serial
  Serial.print("Umidade do solo: ");
  Serial.print(humidity);
  Serial.print("% / reading:");
  Serial.print(sensorValue);
  Serial.println(" ");

  delay(100); // Espere 1 segundo antes de fazer a próxima leitura
}
