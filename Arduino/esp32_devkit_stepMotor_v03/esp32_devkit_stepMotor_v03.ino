#include <Stepper.h>

// Defina os pinos para o controle do motor de passo
const int stepsPerRevolution = 2048; // O motor de passo 28BYJ-48 tem 2048 passos por revolução

const int motorPin1 = 13;    // Pino IN1 conectado ao D5
const int motorPin2 = 12;    // Pino IN2 conectado ao D6
const int motorPin3 = 14;    // Pino IN3 conectado ao D7
const int motorPin4 = 27;    // Pino IN4 conectado ao D8

int qtde = 4;
int i = 0;
int valor = 2048;
int controle = 0;
int velocidade = 12;
int delay_motor = 5;

// Crie um objeto stepper
Stepper myStepper(stepsPerRevolution, motorPin1, motorPin3, motorPin2, motorPin4);

void setup() {
  // Configure o motor para uma velocidade média
  myStepper.setSpeed(velocidade); // Ajuste conforme necessário para a velocidade desejada 
  // 10: minimo
  // 12: maior torque
  // 15: menor torque
 Serial.begin(115200);

}

void loop() {

  voltas(1,-1);
  delay(500);

  voltas(1, 1);
  delay(500);
  
}

void voltas(int qtde, int direcao)  // 1 = direita; -1 = esquerda
{
  int i = 0;
  int sentido = -1;
  // liga motor
  digitalWrite(motorPin1,HIGH);
  digitalWrite(motorPin2,HIGH);
  digitalWrite(motorPin3,HIGH);
  digitalWrite(motorPin4,HIGH);

  for (i = 0; i < qtde; i++) {
    myStepper.step((stepsPerRevolution / 1) * (sentido * direcao));
    delay(delay_motor);

  // desliga motor
  digitalWrite(motorPin1,LOW);
  digitalWrite(motorPin2,LOW);
  digitalWrite(motorPin3,LOW);
  digitalWrite(motorPin4,LOW);

  }
}