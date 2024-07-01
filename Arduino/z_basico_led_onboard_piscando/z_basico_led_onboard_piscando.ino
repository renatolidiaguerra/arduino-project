

const int buttonPin = 13;  // the number of the pushbutton pin

const int ledPin = 2;    // the number of the LED pin
const int releA = 12;
const int releB = 27;

// variables will change:
int buttonState = 0;  // variable for reading the pushbutton status

void setup() {
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);

  
  Serial.begin(115200);
  Serial.println("Iniciando");
}

void loop() {
  // read the state of the pushbutton value:
    digitalWrite(ledPin, HIGH);

    delay(1000);
    digitalWrite(ledPin, LOW);
    delay(1000);
   

    Serial.println("pisca!");
  
}
