#include <Stepper.h>

int stepsPerRevolution = 4096;  //2048  change this to fit the number of steps per revolution 4096-2048
int velocidade = 1; // 11
int delay_motor = 50; // 100

#define IN1 D5
#define IN2 D6
#define IN3 D7
#define IN4 D8

// initialize the stepper library
Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4);

void setup() {
  myStepper.setSpeed(velocidade);
  Serial.begin(9600);
  Serial.println("\n\n\n Iniciando...");
}

void loop() {
    Serial.println("Rotating Motor in opposite direction...");
    // voltas(1,-1);
    // delay(15000);

    myStepper.step(stepsPerRevolution);

    Serial.println("Girando para direita");
    // voltas(1,1);
    // delay(3000);

    delay(5000);
}

void voltas(int qtde, int direcao)  // 1 = direita; -1 = esquerda
{
  int i = 0;
  int sentido = -1;
  // liga motor
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,HIGH);

  for (i = 0; i < qtde; i++) {
    myStepper.step((stepsPerRevolution / 1) * (sentido * direcao));
    delay(delay_motor);

  // desliga motor
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,LOW);

  }
}
// void loop() {
//   for (int var = 0; var < 5; var++) {
//     myStepper.step(stepsPerRevolution);
//     delay(10);
//   }
//   delay(1000);

//   for (int var = 0; var < 3; var++) {
//     myStepper.step(-stepsPerRevolution);
//     delay(10);
//   }
//   delay(500);
// }