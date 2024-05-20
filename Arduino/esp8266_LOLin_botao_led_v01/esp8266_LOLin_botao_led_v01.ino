
int botao1  = D6;
int led     = D7;      // ok, funcionou
int led_board = D2;

bool LedState = 0;
int buttonState = 0;
bool guardaEstado = LOW;

void setup() {

  Serial.begin(115200); /*Baud rate for serial communication*/

  delay(100);
  Serial.println("Iniciando");

  pinMode(led, OUTPUT);
  pinMode(botao1, INPUT);

  digitalWrite(led,LOW);
  
}

void loop() {

    buttonState = digitalRead(botao1);

    if (buttonState == HIGH) {
      guardaEstado = !guardaEstado;
      delay(250);
      Serial.println("Botao 1");
    }

    digitalWrite(led,guardaEstado);

}