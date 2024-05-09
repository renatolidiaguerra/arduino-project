// #include <IRrecv.h>
// #include <IRremoteESP8266.h>
#include <IRremote.h>

#include <Stepper.h>

IRrecv irrecv(8);
decode_results results;

int pinLed = 11;
int velocidade = 8;
int posicao = 1;                // 1= fechado down    2=medio    3=fechado up
int stepsPerRevolution = 2048;  //2048  change this to fit the number of steps per revolution 4096-2048
int delay_motor = 250;          // delay entre cada rotação/volta      inicial=100

// int voltaMinima = 4;            // quantas rotações por movimento - deprecated

int gap3_1  = 10;               // voltas da posicao 1 para a posicao 2
int gap1_3  = 10;               // voltas da posicao 2 para a posicao 3

int gap1_2  = 5;
int gap2_3  = 5;

int gap3_2  = 8;
int gap2_1  = 2;

bool inicio = false;


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

  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,LOW);

  long ii = 0;
  long iix = 60000;
  bool secondLoop = false;

  Serial.println("Aguardando comando inicial...");

  while (inicio == false) {
    // Serial.print(".");   
    if (irrecv.decode()) {
      Serial.println(irrecv.decodedIRData.decodedRawData, HEX);
      // Serial.println(irrecv.decodedIRData.decodedRawData, HEX);
    
      if (irrecv.decodedIRData.decodedRawData == 0xE31CFF00) {  // ok
        secondLoop = true;
      }

      if (secondLoop == true && inicio == false) {
        if (irrecv.decodedIRData.decodedRawData == 0xBA45FF00) {  // 1
          posicao = 1; 
          inicio = true;
          digitalWrite(pinLed, HIGH);
          delay(3000);
          digitalWrite(pinLed, LOW);
          delay(1000);
          digitalWrite(pinLed, HIGH);
          delay(1000);
          digitalWrite(pinLed, LOW);
          delay(3000);
        }
        if (irrecv.decodedIRData.decodedRawData == 0xB946FF00) {  // 2
          posicao = 2; 
          inicio = true;
          digitalWrite(pinLed, HIGH);
          delay(3000);
          digitalWrite(pinLed, LOW);
          delay(1000);
          digitalWrite(pinLed, HIGH);
          delay(1000);
          digitalWrite(pinLed, LOW);
          delay(1000);
          digitalWrite(pinLed, HIGH);
          delay(1000);
          digitalWrite(pinLed, LOW);
          delay(3000);
        }
        if (irrecv.decodedIRData.decodedRawData == 0xB847FF00) {  // 3
          posicao = 3; 
          inicio = true;
          digitalWrite(pinLed, HIGH);
          delay(3000);
          digitalWrite(pinLed, LOW);
          delay(1000);
          digitalWrite(pinLed, HIGH);
          delay(1000);
          digitalWrite(pinLed, LOW);
          delay(1000);
          digitalWrite(pinLed, HIGH);
          delay(1000);
          digitalWrite(pinLed, LOW);
          delay(1000);
          digitalWrite(pinLed, HIGH);
          delay(1000);
          digitalWrite(pinLed, LOW);
          delay(3000);
        }
      }
      delay(250);
      irrecv.resume();
    }

    ii = ii + 1;
    if (secondLoop == true) {
      if (ii < iix / 2) {
        digitalWrite(pinLed, LOW);
      } else {
        digitalWrite(pinLed, HIGH);
      }
      if (ii == iix) {
        ii = 0;
      }       
    } else {
            if (ii < iix / 8) {
        digitalWrite(pinLed, LOW);
      } else {
        digitalWrite(pinLed, HIGH);
      }
      if (ii == iix) {
        ii = 0;
      }
    }
  }
  
  digitalWrite(pinLed, HIGH);
  Serial.println("desligando luz");
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

    if (irrecv.decodedIRData.decodedRawData == 0xBA45FF00) {  // 1
      Serial.println("comando = 1");
      if (posicao == 2) {
        voltas(gap2_1, -1);
      } else {
        if (posicao == 3) {
          voltas(gap3_1, -1);
        }
      }
      posicao = 1;
      // voltas(1,1);
    }
    if (irrecv.decodedIRData.decodedRawData == 0xB946FF00) {  // 2
      Serial.println("comando = 2");
      // voltas(5,1);
      if (posicao == 1) {
        voltas(gap1_2, 1);
      } else {
        if (posicao == 3) {
          voltas(gap3_2, -1);
        }
      }
      posicao = 2;
    }
    if (irrecv.decodedIRData.decodedRawData == 0xB847FF00) {  // 3
      Serial.println("comando = 3");
      // voltas(2,-1);
      if (posicao == 2) {
        voltas(gap2_3, 1);
      } else {
        if (posicao == 1) {
          voltas(gap1_3, 1);
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
  if (irrecv.decodedIRData.decodedRawData == 0xE718FF00) {  // up 
      Serial.println("meia-volta em direcao ao 3");
      meiavolta(1);
    }
    if (irrecv.decodedIRData.decodedRawData == 0xAD52FF00) {  // down
      Serial.println("meia volta em direcao ao 1");
      meiavolta(-1);
    }

  //myStepper.step(stepsPerRevolution);
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
void meiavolta(int direcao)  // 1 = direita; -1 = esquerda
{
  int sentido = -1;
  // liga motor
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,HIGH);

  myStepper.step((stepsPerRevolution / 2) * (sentido * direcao));

  // desliga motor
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,LOW);

}
