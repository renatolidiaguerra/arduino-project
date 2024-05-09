// #include <ESP32Servo.h>
#include <Servo.h>

// #include <IRrecv.h>
#include <IRremote.h>
int pinSensor = 8;
boolean ligado = false;
IRrecv irrecv(8);
decode_results results;

int ledPin = 3;

    // x-y: 0-25; 20-100; 40-100; 40-20

float min_x = 20;           //  20
float max_x = 59;           //  59
float min_y = 25;           //  25
float max_y = 100;          //  100

int min_freeze = 200;       //  200
int max_freeze = 3000;      //  3000
float minimal_movement = 4; //  4

// finding center of square for starting point
int random_delay;
unsigned long startMillis;
unsigned long result;
float x_position = min_x + (max_x - min_x)/2;
float y_position = min_y + (max_y - min_y)/2; 
float x_old_position = x_position;
float y_old_position = y_position;
float x_new_position;
float y_new_position;
float x_speed;
float y_speed;
int movement_time;
int moviment_speed;

unsigned long durationTimer;
unsigned long constMinuto    = 60000;                    // (1000ms x 60 sec/min)   = 1 minuto
unsigned long tempoTotal     = constMinuto * 120;         // (constMinuto * 60)*2;     // ( x 60 min/hr )         = 2 horas de duração
unsigned long tempoLigado    = constMinuto * 10;           //                         = 10 minutos
unsigned long tempoDesligado = constMinuto * 10;          //                         = 10 minutos
unsigned long startMovimento = 0;
unsigned long startParado    = 0;
boolean       flagMovimento;

// Instantiating two servos
Servo x_servo;  
Servo y_servo;
int pos = 0;

int pinY = 10;
int pinX = 9;

void setup() {
  y_servo.attach(pinY);  // attaches the y servo on pin 6 to the servo object
  x_servo.attach(pinX);  // attaches the x servo on pin 9 to the servo object

  Serial.begin(9600);

  //Place the servos in the center at the beginning 
  y_servo.write(y_position); 
  x_servo.write(x_position);     

  pinMode(ledPin, OUTPUT);

  // inicializa IRReceiver
  irrecv.enableIRIn();  // Start the receiver

  startMillis = millis();
  //durationTimer = millis();
  Serial.println("Iniciando...");
  campo();
  // linha();
}

void loop() {
    
    if (irrecv.decode()) {
      if (irrecv.decodedIRData.decodedRawData == 0xE31CFF00) {   // ok
        Serial.println("Pressionado ON/OFF");
        ligado = !ligado;

        if (ligado) {
          Serial.println("Inicianto timer pelo IR");
          durationTimer = millis();
          flagMovimento = true;
          startMovimento = millis();
          digitalWrite(ledPin, HIGH);
        } else {
          digitalWrite(ledPin, LOW);
        }
      }

      // if (irrecv.decodedIRData.decodedRawData == 0xBA45FF00) {    // 1
      //   Serial.println("Pressionado 1 - exibindo range");
      //   campo();
      // }

      delay(100);
      irrecv.resume();
      
    }

    if (ligado) {
      
      // se executando a mais de 5 minutos, flagMovimento=nao -> pára de movimentar
      if (flagMovimento) {
        if ((millis() - startMovimento) >= tempoLigado) {
          Serial.print("off;");
          digitalWrite(ledPin, LOW);
          flagMovimento = false;
          startParado = millis();
        }    
      } else {
        // se parado a mais de 15 minutos, flagMovimento=sim -> volta a movimentar
        if ((millis() - startParado) >= tempoDesligado) {
          Serial.println("[on]");
          digitalWrite(ledPin, HIGH);
          flagMovimento = true;
          startMovimento = millis();
        } 
      }
      // se espera ultrapassou random_delay, movimenta
      if (flagMovimento) {
        if ((millis() - startMillis) >= random_delay) {
          Serial.print("/");
          move_laser();
          startMillis = millis();
        } 
      }
      
      // se duração ultrapassou tempo total definido, desliga tudo
      if ((millis() - durationTimer) >= tempoTotal ) { 
        Serial.println("desligando por tempo");
        digitalWrite(ledPin, LOW);
        ligado = false; 
      }

    }
}

void move_laser() 
{
  random_delay = random(min_freeze, max_freeze);
  movement_time = random(40,80);   // velocidade do movimento do ponto: original: 20,60  ideal: 40,80
  moviment_speed = random(10,40);  // distancia entre os proximos pontos  (quanto maior, mais lento) original: 10,40 ; 4,10
  x_new_position = random(min_x+minimal_movement, max_x-minimal_movement);
  y_new_position = random(min_y+minimal_movement, max_y-minimal_movement);
  
  if( (y_new_position > y_old_position) && (abs(y_new_position - y_old_position) < 5 )) {
    y_new_position = y_new_position + minimal_movement;
  }  else if ( (y_new_position < y_old_position) && (abs(y_new_position - y_old_position) < 5 )) {
    y_new_position = y_new_position - minimal_movement;
  }
  
  if( (x_new_position > x_old_position) && (abs(x_new_position - x_old_position) < 5 )) {
    x_new_position = x_new_position + minimal_movement;
  }  else if ( (x_new_position < x_old_position) && (abs(x_new_position - x_old_position) < 5 )) {
    x_new_position = x_new_position - minimal_movement;
  }
  
  x_speed = (x_new_position - x_old_position)/movement_time;
  y_speed = (y_new_position - y_old_position)/movement_time;  
  for (pos = 0; pos < movement_time; pos += 1) { 
      x_position = x_position + x_speed;
      y_position = y_position + y_speed;
      x_servo.write(x_position);  
      y_servo.write(y_position);                    
    delay(moviment_speed); 
  }
  x_old_position = x_new_position;
  y_old_position = y_new_position; 
  //delay(random_delay);   // substituido por delay interno com receiver de IR
}
void campo() {
    digitalWrite(ledPin, HIGH);
    int repet = 2;
    int veloc = 500;
    if (true) {                             // quadrado completo
      for (pos = 0; pos < repet; pos += 1) { 
        x_servo.write(min_x);  
        y_servo.write(min_y);                    
        delay(veloc); 
        x_servo.write(min_x);  
        y_servo.write(max_y);                    
        delay(veloc); 
        x_servo.write(max_x);  
        y_servo.write(max_y);                    
        delay(veloc); 
        x_servo.write(max_x);  
        y_servo.write(min_y);                    
        delay(veloc); 
      }
    }
    if (true) {                             // lado 1
      for (pos = 0; pos < repet; pos += 1) { 
        x_servo.write(min_x);  
        y_servo.write(min_y);                    
        delay(veloc); 
        x_servo.write(min_x);  
        y_servo.write(max_y);                    
        delay(veloc); 
      }
    }
    if (true) {                             // lado 2
      for (pos = 0; pos < repet; pos += 1) { 
        x_servo.write(min_x);  
        y_servo.write(max_y);                    
        delay(veloc); 
        x_servo.write(max_x);  
        y_servo.write(max_y);                    
        delay(veloc); 
      }
    }
    if (true) {                             // lado 3
      for (pos = 0; pos < repet; pos += 1) { 
        x_servo.write(max_x);  
        y_servo.write(max_y);                    
        delay(veloc); 
        x_servo.write(max_x);  
        y_servo.write(min_y);                    
        delay(veloc); 
      }
    }
    if (true) {                             // lado 4
      for (pos = 0; pos < repet; pos += 1) { 
        x_servo.write(max_x);  
        y_servo.write(min_y);                    
        delay(veloc); 
        x_servo.write(min_x);  
        y_servo.write(min_y);                    
        delay(veloc); 
      }
    }
  digitalWrite(ledPin, LOW);
}
void linha() {
  digitalWrite(ledPin, HIGH);

  for (pos = 0; pos < 10; pos += 1) { 
    x_servo.write(min_x);  
    y_servo.write(min_y);                    
    delay(200); 
    x_servo.write(max_x);  
    y_servo.write(min_y);                    
    delay(1000); 
  }
  for (pos = 0; pos < 100; pos += 1) { 
    x_servo.write(min_x);  // 40
    y_servo.write(min_y);  // 100                   

    // x-y: 0-25; 20-100; 40-100; 40-20
    delay(5000); 
  }
  digitalWrite(ledPin, LOW);
}