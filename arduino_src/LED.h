/**************************************************************
* "LED.h" - a simple library for multiColor LED
 **************************************************************/
#ifndef LED_H
#define LED_H

#include <Arduino.h> // need this library to include delay()

//LED class with initialization of methods and variables, private and public where appropriate.
class LED {
public:
  LED(int ledPin);
  void turnOn();
  void turnOff();
  void strobe(int time);
private:
  int ledPin; // pin for LED (such as D3)
};

#endif