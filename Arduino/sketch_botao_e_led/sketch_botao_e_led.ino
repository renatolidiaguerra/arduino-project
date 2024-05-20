

const int buttonPin = 13;  // the number of the pushbutton pin

const int ledPin = 2;    // the number of the LED pin
const int releA = 12;
const int releB = 27;

// variables will change:
int buttonState = 0;  // variable for reading the pushbutton status

void setup() {
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  pinMode(releA, OUTPUT);
  pinMode(releB, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);

  // for (int i = 0; i < 2; i++) {
  //   digitalWrite(ledPin, HIGH);
  //   delay(1000);
  //   digitalWrite(ledPin, LOW);
  //   delay(1000);
  // }
  
  Serial.begin(115200);
  Serial.println("Iniciando");
}

void loop() {
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    // turn LED on:
    digitalWrite(ledPin, HIGH);
    digitalWrite(releB, LOW);
    Serial.println(" --  Ligado -- ");
  } else {
    // turn LED off:
    digitalWrite(ledPin, LOW);
    digitalWrite(releB, HIGH);
    Serial.println("Desligado");
  }
}
