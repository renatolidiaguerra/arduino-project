#include "pitches.h"
const int BUZZZER_PIN = D5; // The ESP8266 pin connected to piezo buzzer
int noteDuration = 0;

// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

void setup() {

  Serial.begin(115200);
  // // iterate over the notes of the melody:
  // for (int thisNote = 0; thisNote < 8; thisNote++) {

  //   // to calculate the note duration, take one second divided by the note type.
  //   //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
  //   noteDuration = 1000 / noteDurations[thisNote];
  //   tone(BUZZZER_PIN, melody[thisNote], noteDuration);

  //   // to distinguish the notes, set a minimum time between them.
  //   // The note's duration + 30% seems to work well:
  //   int pauseBetweenNotes = noteDuration * 2.30;
  //   delay(pauseBetweenNotes);
  //   // stop the tone playing:
  //   noTone(BUZZZER_PIN);
  // }
}

void loop() {
  // no need to repeat the melody.
  //noteDuration = 1000 / 4;
      tone(BUZZZER_PIN, 784, 250 );
  delay(1000);
// Serial.println("inicio");

//   tocar(NOTE_G1);
// Serial.println("a");
//   tocar(NOTE_G2);
// Serial.println("b");
//   tocar(NOTE_G3);
// Serial.println("c");
//   tocar(NOTE_G4);
// Serial.println("d");
//   tocar(NOTE_G5);
// Serial.println("e");
//   tocar(NOTE_G6);
// Serial.println("f");
//   tocar(NOTE_G7);

  
  // tone(D5, 196, 250 );
  // delay(1500);

  // 39
}

void tocar(char nota) {
   // Serial.println(nota);
  tone(BUZZZER_PIN, nota, 200 );
  delay(300);

  tone(BUZZZER_PIN, nota, 250 );
  delay(1000);
}