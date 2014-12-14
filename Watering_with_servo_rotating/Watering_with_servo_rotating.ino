
#include <Servo.h>

#include "pitches.h"

// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3,NOTE_G3, NOTE_A3, NOTE_G3,0, NOTE_B3, NOTE_C4};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4,4,4,4,4 };


Servo myservo;
Servo botmyservo;
int servoPin = 8;
int botServoPin = 7;
int wateringTime;
int timeBetweenWaterings;

void setup() {
  pinMode(servoPin, OUTPUT);
  pinMode(botServoPin, OUTPUT); 
  Serial.begin(9600); 
  myservo.attach(servoPin);
  botmyservo.attach(botServoPin);
  // time for watering each plant in seconds
  wateringTime = 10;
  // time between each watering sequence. In hours
  timeBetweenWaterings = 24; //should be abour two days
  // Initial state of servos
  myservo.write(45);
  botmyservo.write(0); 
  delay(2000);
}

void loop() {
  playTune(); //indicates start of watering, not necessary
  delay(30000); //30 seconds delay between tune and watering
  for (int i = 0; i < 10 ; i += 1) {
    if (i > 7) {
      botmyservo.write(i*10 + 5); // added 5 to compensate for spread in holes
    }
    else {
      botmyservo.write(i*10); // bottom servo rotates by 10 degrees each iteration
    }
    delay(500);
    myservo.write(0); // watering position for top servo
    for (int j = 0 ; j < wateringTime ; j += 1) {
      delay(1000); // stay in watering position
    }
    myservo.write(45); // end watering by elevating hose with top servo
    delay(1000);
  }
  for (int k = 90 ; k >= 0 ; k -= 1) {
    botmyservo.write(k); // attempt to slow down rotation
    delay(3);
  }
  for (int l = 0 ; l < timeBetweenWaterings ; l += 1) {
    for (int m = 0; m < 3600; m += 1) {
      delay(1000); // delays for the given number of hours 
    }
  }
  wateringTime += 2; // As the preassure drops in the system, I have to compensate by increasing the watering time
}

void playTune(){
  for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second 
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000/noteDurations[thisNote];
    tone(4, melody[thisNote],noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(4);
  }
}

