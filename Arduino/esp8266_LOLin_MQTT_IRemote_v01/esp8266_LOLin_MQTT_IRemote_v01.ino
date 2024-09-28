#include <IRrecv.h>
#include <IRremoteESP8266.h>

IRrecv irrecv(D5);
decode_results results;

const int ledVerde = 12;
const int ledAmarelo = 27;
const int rele = 26;

void setup() {

  pinMode(rele, OUTPUT);
  pinMode(ledVerde, OUTPUT);
  pinMode(ledAmarelo, OUTPUT);
  irrecv.enableIRIn();  // Start the receiver
  Serial.begin(9600);
  digitalWrite(rele, LOW);
}

// codigos
// 1= FFA25D
// 2= FF629D
// 3= FFE21D
// 4= 
// 5= 
// 6= 
// 7= 
// 8= 
// 9= 
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

    Serial.print(results.value, HEX);

    if (results.value == 0xFF18E7) {   // up
      digitalWrite(ledAmarelo, HIGH);
      Serial.println("=up");
    }

    if (results.value == 0xFF4AB5) {   // down
      digitalWrite(ledVerde, HIGH);
      Serial.println("=down");
    }    
    if (results.value == 0xFFA25D) {   // 1
      Serial.println("=1");
    } 
    if (results.value == 0xFF629D) {   // 1
      Serial.println("=2");
    } 
    if (results.value == 0xFFE21D) {   // 
      Serial.println("=3");
    } 
    if (results.value == 0xFF22DD) {   // 
      Serial.println("=4");
    } 
    if (results.value == 0xFF02FD) {   // 
      Serial.println("=5");
    } 
    if (results.value == 0xFFC23D) {   // 
      Serial.println("=6");
    } 
    if (results.value == 0xFFE01F) {   // 
      Serial.println("=7");
    } 
    if (results.value == 0xFFA857) {   // 
      Serial.println("=8");
    } 
    if (results.value == 0xFF906F) {   // 
      Serial.println("=9");
    } 
    if (results.value == 0xFF9867) {   // 
      Serial.println("=0");
    } 
    if (results.value == 0xFFB04F) {   // 
      Serial.println("=#");
    } 
    if (results.value == 0xFF38C7) {   // 
      Serial.println("=ok");
    } 

    if (results.value == 0xFF10EF) {   // left
      Serial.println("=left");
    }    
    if (results.value == 0xFF5AA5) {   // right
      Serial.println("=right");
    }    
    if (results.value == 0xFF6897) {   // estrelas
      Serial.println("=star");
    }    

    Serial.println("\n-----------------");
    delay(100);
    irrecv.resume();

  }
}