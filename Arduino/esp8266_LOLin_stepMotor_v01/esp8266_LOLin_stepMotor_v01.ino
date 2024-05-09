#include <AccelStepper.h>

const int steps_per_rev = 2048;  //  2048
#define IN1 D5
#define IN2 D6
#define IN3 D7
#define IN4 D8

AccelStepper motor(AccelStepper::HALF4WIRE, IN1, IN3, IN2, IN4);

void setup() {
  Serial.begin(115200);

  motor.setMaxSpeed(100);     // 700
  motor.setAcceleration(10);  // 200

  motor.moveTo(steps_per_rev);
}

void loop() {

  if (motor.distanceToGo() == 0){
    motor.moveTo(-motor.currentPosition());
    Serial.println("Rotating Motor in opposite direction...");
  }
  motor.run();


  // // for (int var = 0; var < 5; var++) {
  //   motor.moveTo(steps_per_rev);
  //   motor.run();
  // }

      motor.moveTo(steps_per_rev / 2);
    motor.run();

  // delay(1000);
  // for (int var = 0; var < 1; var++) {
  //   motor.moveTo(steps_per_rev);
  //   motor.run();
  // }
  // delay(500);
}
