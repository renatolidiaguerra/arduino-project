// #include <IRrecv.h>
// #include <IRremoteESP8266.h>
#include <IRremote.h>

#include <Stepper.h>

IRrecv irrecv(8);
decode_results results;

int pinLed = 25;
int velocidade = 15;
int posicao = 1;                // 1= fechado down    2=medio    3=fechado up
int stepsPerRevolution = 2048;  //2048  change this to fit the number of steps per revolution 4096-2048

int voltaMinima = 3;

// proximos passos:
// - colocar ajuste fino com direcionais
// - criar modo setup:
//    - ligar modo setup;
//    - selecionar posicao a ser definida;
//    - com direcionais, selecionar voltas;
//    - com OK, confirmar e gravar;
//    - mas dai quando reinicia, perde...
// - criar botao loop 1 - 2 - 3 - 2 - 1 - 2 ...
// 


// ULN2003 Motor Driver Pins
#define IN1 2
#define IN2 3
#define IN3 4
#define IN4 5

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
  if (irrecv.decode()) {

    Serial.println(irrecv.decodedIRData.decodedRawData, HEX);

    if (irrecv.decodedIRData.decodedRawData == 0xE718FF00) {  // up
      velocidade = velocidade + 5;
      myStepper.setSpeed(velocidade);
      Serial.println(velocidade);
      Serial.println("faster");
    }

    if (irrecv.decodedIRData.decodedRawData == 0xAD52FF00) {  // down
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
    if (irrecv.decodedIRData.decodedRawData == 0xBA45FF00) {  // 1
      Serial.println("comando = 1");
      if (posicao == 2) {
        voltas(voltaMinima*1, -1);
      } else {
        if (posicao == 3) {
          voltas(voltaMinima*2, -1);
        }
      }
      posicao = 1;
      // voltas(1,1);
    }
    if (irrecv.decodedIRData.decodedRawData == 0xB946FF00) {  // 2
      Serial.println("comando = 2");
      // voltas(5,1);
      if (posicao == 1) {
        voltas(voltaMinima*1, 1);
      } else {
        if (posicao == 3) {
          voltas(voltaMinima*1, -1);
        }
      }
      posicao = 2;
    }
    if (irrecv.decodedIRData.decodedRawData == 0xB847FF00) {  // 3
      Serial.println("comando = 3");
      // voltas(2,-1);
      if (posicao == 2) {
        voltas(voltaMinima*1, 1);
      } else {
        if (posicao == 1) {
          voltas(voltaMinima*2, 1);
        }
      }
      posicao = 3;
    }
    if (irrecv.decodedIRData.decodedRawData == 0xE916FF00) {   // * = reset/start/init
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
