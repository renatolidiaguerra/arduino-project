#include <IRrecv.h>
#include <IRremoteESP8266.h>

#include <Stepper.h>

IRrecv irrecv(26);
decode_results results;

int pinLed = 25;
int velocidade = 15;
int posicao = 1;                // 1= fechado down    2=medio    3=fechado up
int stepsPerRevolution = 2048;  //2048  change this to fit the number of steps per revolution 4096-2048

// ULN2003 Motor Driver Pins
#define IN1 13
#define IN2 12
#define IN3 14
#define IN4 27

// initialize the stepper library
Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4);

void setup() {

  myStepper.setSpeed(velocidade);
  pinMode(pinLed, OUTPUT);
  irrecv.enableIRIn();  // Start the receiver
  Serial.begin(115200);
}

// codigos
// ok=FF38C7
// up=FF18E7
// dw=FF4AB5
// lf=FF10EF
// rg=FF5AA5

// The repeating section of the code
void loop() {
  // Check if the IR code has been received.
  if (irrecv.decode(&results)) {

    Serial.println(results.value, HEX);

    if (results.value == 0xFF18E7) {  // up
      velocidade = velocidade + 5;
      myStepper.setSpeed(velocidade);
      Serial.println(velocidade);
      Serial.println("faster");
    }

    if (results.value == 0xFF4AB5) {  // down
      velocidade = velocidade - 5;
      if (velocidade < 0) { velocidade = 0; }
      myStepper.setSpeed(velocidade);
      Serial.println(velocidade);
      Serial.println("lower");
    }

    // if (results.value == 0xFF10EF) {   // left
    //   stepsPerRevolution = 1;
    //   Serial.println("left");
    // }
    // if (results.value == 0xFF5AA5) {   // right
    //   stepsPerRevolution = -1;
    //   Serial.println("right");
    // }
    if (results.value == 0xFFA25D) {  // 1
      Serial.println("comando = 1");
      if (posicao == 2) {
        voltas(1, -1);
      } else {
        if (posicao == 3) {
          voltas(2, -1);
        }
      }
      posicao = 1;
      // voltas(1,1);
    }
    if (results.value == 0xFF629D) {  // 2
      Serial.println("comando = 2");
      // voltas(5,1);
      if (posicao == 1) {
        voltas(1, 1);
      } else {
        if (posicao == 3) {
          voltas(1, -1);
        }
      }
      posicao = 2;
    }
    if (results.value == 0xFFE21D) {  // 3
      Serial.println("comando = 3");
      // voltas(2,-1);
      if (posicao == 2) {
        voltas(1, 1);
      } else {
        if (posicao == 1) {
          voltas(2, 1);
        }
      }
      posicao = 3;
    }
    if (results.value == 0xFF9867) {
      posicao = 1;
      digitalWrite(pinLed, HIGH);
      delay(750);
      digitalWrite(pinLed, LOW);
    }
    delay(100);
    irrecv.resume();
  }
  //myStepper.step(stepsPerRevolution);
}

void voltas(int qtde, int direcao)  // 1 = direita; -1 = esquerda
{
  int i = 0;
  int sentido = -1;
  for (i = 0; i < qtde; i++) {
    myStepper.step((stepsPerRevolution / 1) * (sentido * direcao));
    delay(100);
  }
}
