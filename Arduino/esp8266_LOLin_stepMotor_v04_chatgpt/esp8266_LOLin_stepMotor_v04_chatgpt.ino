#include <Stepper.h>

// Defina os pinos para o controle do motor de passo
const int motorSteps = 2048; // O motor de passo 28BYJ-48 tem 2048 passos por revolução
// const int motorPin1 = D5;    // Pino IN1 conectado ao D5
// const int motorPin2 = D7;    // Pino IN2 conectado ao D6
// const int motorPin3 = D6;    // Pino IN3 conectado ao D7
// const int motorPin4 = D8;    // Pino IN4 conectado ao D8

const int motorPin1 = 13;    // Pino IN1 conectado ao D5
const int motorPin2 = 12;    // Pino IN2 conectado ao D6
const int motorPin3 = 14;    // Pino IN3 conectado ao D7
const int motorPin4 = 27;    // Pino IN4 conectado ao D8

int qtde = 4;
int i = 0;
int valor = 2048;
int controle = 0;

// Crie um objeto stepper
Stepper stepper(motorSteps, motorPin1, motorPin3, motorPin2, motorPin4);

void setup() {
  // Configure o motor para uma velocidade média
  stepper.setSpeed(12); // Ajuste conforme necessário para a velocidade desejada 
  // 10: minimo
  // 12: maior torque
  // 15: menor torque
 Serial.begin(115200);
 // for (i = 0; i < qtde; i++) {

 //   delay(10);
 // }

}

void loop() {

  if (controle == 1) {
    stepper.step(valor); 
    Serial.println("entrou no 1");
    controle = 2;
  } else {
    stepper.step(valor * -1);
    controle = 1;
    Serial.println("entrou  no  else");
  }
  delay(1000);

  // Não há nada para fazer no loop após o movimento do motor ser completado
}
