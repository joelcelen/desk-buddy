#include "Buzzer.h"

// Constructor
Buzzer::Buzzer() {
  time = 1000; // default buzz time in ms
}

// buzzer method -- buzzes for 1 second to notify user
void Buzzer::notify() {
  analogWrite(WIO_BUZZER, 128);
  delay(this->time);
  analogWrite(WIO_BUZZER, 0);
}

// buzzer method -- buzzes for user specified time length in ms
void Buzzer::notify(int time) {
  analogWrite(WIO_BUZZER, 128);
  delay(time);
  analogWrite(WIO_BUZZER, 0);
}

// loud buzzer method -- buzzes loudly for 1 second to notify user
void Buzzer::notifyLoudly() {
  analogWrite(WIO_BUZZER, 255);
  delay(1000);
  analogWrite(WIO_BUZZER, 0);
}