// #include <ESP32Servo.h>
#include <Servo.h>

// #include <IRrecv.h>
#include <IRremote.h>
int pinSensor = 8;
boolean ligado = false;
IRrecv irrecv(8);
decode_results results;

int ledPin = 3;

// X servo angle will stay in [min_x, max_x] range
// Y servo angle will stay in [min_y, max_y] range
// to be ajsuted to the size of your living room

float min_x = 5;
float max_x = 50;
float min_y = 5;
float max_y = 35;
int min_freeze = 200;
int max_freeze = 3000;
float minimal_movement = 5;

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
unsigned long tempoLigado = (3600000)*2; // (1000ms x 60 sec/min x 60 min/hr )  *  2 horas de duração

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
}

void loop() {
    
    if (irrecv.decode()) {
      if (irrecv.decodedIRData.decodedRawData == 0xE31CFF00) {   // ok
        Serial.println("Pressionado ON/OFF");
        ligado = !ligado;

        if (ligado) {
          durationTimer = millis();
          Serial.println("Inicianto timer pelo IR");
          digitalWrite(ledPin, HIGH);
        } else {
          digitalWrite(ledPin, LOW);
        }
      }
      delay(100);
      irrecv.resume();
    }

    if (ligado && (millis() - startMillis >= random_delay)) {
      Serial.print("/");
      move_laser();
      startMillis = millis();
    } 

    if ( ligado  &&  ((millis() - durationTimer) >= tempoLigado )) { 
      Serial.println("desligando por tempo");
      digitalWrite(ledPin, LOW);
      ligado = false; 
    }
}

void move_laser() 
{
  random_delay = random(min_freeze, max_freeze);
  movement_time = random(20,60);
  moviment_speed = random(4,10);  // faixa de velocidade do movimento ? (quanto maior, mais lento) original: 10,40
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