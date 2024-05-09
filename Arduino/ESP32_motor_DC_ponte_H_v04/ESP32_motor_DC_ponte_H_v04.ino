
// ------------------
// LOLIN D1 mini lite
// ------------------


#include <Arduino.h>

// Definição dos pinos
const int motorDIR = 4;  //  d3 Saída INT3 do L298N (motorDIR)
const int motorESQ = 5;  // d6 Saída INT4 do L298N (motorESQ)
const int botao = 22;     // d7
int buttonState = 0;                                                                                                
int sentido = LOW;
int qtde = 1;
int previousState = HIGH;

void setup() {

  Serial.begin(115200);

  pinMode(motorDIR, OUTPUT);
  pinMode(motorESQ, OUTPUT);

  pinMode(botao, INPUT_PULLUP); 

  Serial.println("\n\n\nIniciando....");

  // testeInicial ();

 parar();
}

void loop() {

  buttonState = digitalRead(botao);

  if (buttonState == LOW) {

    Serial.println("pressionado...");
    previousState = LOW;

    // sentido = !sentido;
    voltas(sentido, qtde);
  }

  if (buttonState == HIGH && previousState == LOW) {
    sentido = !sentido;
    previousState = HIGH;
    parar();
  }
  delay(100);
}

void voltas(int direcao, int qtde) {

  // movimenta o motor: 1 = horario
  //                    0 = anti-horario
  //
  if (direcao == HIGH) {
    digitalWrite(motorDIR, LOW);
    digitalWrite(motorESQ, HIGH);
    Serial.println("horario");

  } else {
    digitalWrite(motorDIR, HIGH);
    digitalWrite(motorESQ, LOW);
    Serial.println("anti-horario");
  }
  // controle de voltas
  delay(qtde * 1000);  // 12v
  // para o motor
 // parar();
}

void parar() {
  Serial.println("executado parar()");
  digitalWrite(motorDIR, HIGH);
  digitalWrite(motorESQ, HIGH);
}

void testeInicial ()  {
    Serial.print("HIGH-HIGH");
  digitalWrite(motorDIR, HIGH);
  digitalWrite(motorESQ, HIGH);
  delay(5000);

  delay(2000);

  Serial.print("HIGH-LOW");
  digitalWrite(motorDIR, HIGH);
  digitalWrite(motorESQ, LOW);
  delay(5000);

  delay(2000);

  Serial.print("LOW-HIGH");
  digitalWrite(motorDIR, LOW);
  digitalWrite(motorESQ, HIGH);
  delay(5000);

  delay(2000);

  Serial.print("LOW-LOW");
  digitalWrite(motorDIR, LOW);
  digitalWrite(motorESQ, LOW);
  delay(5000);

  delay(2000);

}