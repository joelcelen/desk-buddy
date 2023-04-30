#ifndef BUZZER_H
#define BUZZER_H

#include <SPI.h>

//Buzzer class with initialization of methods and variables, private and public where appropriate.
class Buzzer {
public:
  Buzzer();
  void notify();
  void notify(int time);
  void notifyLoudly();
private:
  int time; // time in ms to buzz
};

#endif