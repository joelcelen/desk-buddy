#include "LED.h"

// Constructor
LED::LED(int ledPin) {
  this->ledPin = ledPin;            // set pin for LED digital read/write operations
}

// turnOn method
void LED::turnOn() {
  digitalWrite(ledPin, HIGH);
}

// turnOff method
void LED::turnOff() {
  digitalWrite(ledPin, LOW);
}

// strobe method
void LED::strobe(int time) {
  turnOn();
  delay(time);
  turnOff();
}