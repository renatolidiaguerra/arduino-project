#include <IRrecv.h>
#include <IRremoteESP8266.h>

IRrecv irrecv(35);
decode_results results;

void setup() {

  irrecv.enableIRIn();  // Start the receiver
  Serial.begin(115200);
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
    // Display a crude timestamp.
    uint32_t now = millis();
    Serial.println(results.value, HEX);
    delay(1000);
    irrecv.resume();
  }
}