#include <IRrecv.h>
#include <IRremoteESP8266.h>

IRrecv irrecv(26);
decode_results results;

int velocidade = 50;


#include <Stepper.h>

int stepsPerRevolution = 500;  //2048  change this to fit the number of steps per revolution

// ULN2003 Motor Driver Pins
#define IN1 13
#define IN2 12
#define IN3 14
#define IN4 27

// initialize the stepper library
Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4);



void setup() {

  myStepper.setSpeed(velocidade);

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

    if (results.value == 0xFF18E7) {   // up
      velocidade = velocidade + 5;
      Serial.print("velocidade:");
      myStepper.setSpeed(velocidade);
      Serial.println(velocidade);
      Serial.println("faster");
    }

    if (results.value == 0xFF4AB5) {   // down
      velocidade = velocidade - 5;
      if (velocidade < 0) { velocidade = 0;}
      Serial.print("velocidade:");
      myStepper.setSpeed(velocidade);
      Serial.println(velocidade);
      Serial.println("lower");
    }    
    if (results.value == 0xFF10EF) {   // left
      stepsPerRevolution = 1;
      Serial.println("left");
    }    
    if (results.value == 0xFF5AA5) {   // right
      stepsPerRevolution = -1;
      Serial.println("right");
    }    
    delay(100);
    irrecv.resume();

  }
  myStepper.step(stepsPerRevolution);
}