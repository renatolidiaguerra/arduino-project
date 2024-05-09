// #include <IRrecv.h>
#include <IRremote.h>

IRrecv irrecv(8);
decode_results results;

const int ledVerde = 12;
const int ledAmarelo = 27;
const int rele = 26;

void setup() {

  pinMode(rele, OUTPUT);
  pinMode(ledVerde, OUTPUT);
  pinMode(ledAmarelo, OUTPUT);
  irrecv.enableIRIn();  // Start the receiver
  Serial.begin(115200);
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
  if (irrecv.decode()) {

    Serial.println(irrecv.decodedIRData.decodedRawData, HEX);

    if (irrecv.decodedIRData.decodedRawData == 0xFF18E7) {   // up
      digitalWrite(ledAmarelo, HIGH);
      Serial.println("ligado");
    }

    if (irrecv.decodedIRData.decodedRawData == 0xFF4AB5) {   // down
      digitalWrite(ledVerde, HIGH);
      Serial.println("desligado");
    }    
    if (irrecv.decodedIRData.decodedRawData == 0xFFA25D) {   // 1
      digitalWrite(ledVerde, LOW);
      digitalWrite(ledAmarelo, LOW);
    } 
    if (irrecv.decodedIRData.decodedRawData == 0xFF10EF) {   // left
      digitalWrite(rele, HIGH);
      Serial.println("liga");
    }    
    if (irrecv.decodedIRData.decodedRawData == 0xFF5AA5) {   // right
      digitalWrite(rele, LOW);
      Serial.println("desliga");
    }    

    delay(100);
    irrecv.resume();

  }
}