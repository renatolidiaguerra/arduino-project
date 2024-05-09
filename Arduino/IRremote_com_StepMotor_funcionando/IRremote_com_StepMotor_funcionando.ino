#include <IRrecv.h>
#include <IRremoteESP8266.h>
// #include <Stepper.h>

IRrecv irrecv(26);
decode_results results;

const int stepsPerRevolution = 2048;  //2048  change this to fit the number of steps per revolution

// ULN2003 Motor Driver Pins
// #define IN1 13
// #define IN2 12
// #define IN3 14
// #define IN4 27

void setup() {

  irrecv.enableIRIn();  // Start the receiver
  Serial.begin(9600);
}

// codigos
// 1= FFA25D
// 2= FF629D
// 3= FFE21D
// 4= 
// 0=FF9867

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
      Serial.println("faster");
    }
    if (results.value == 0xFF4AB5) {   // down
      Serial.println("slower");
    }    
    if (results.value == 0xFF10EF) {   // left
      Serial.println("esquerda");
    }    
    if (results.value == 0xFF5AA5) {   // right
      Serial.println("direita");
    }    

    delay(500);
    irrecv.resume();

  }
}