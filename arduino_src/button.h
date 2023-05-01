#ifndef BUTTON_H
#define BUTTON_H

#include <SPI.h>

//Button class with initialization of methods and variables, private and public where appropriate.
class Button {
public:
  Button();
  void delayUntilPressed();
  bool checkState();
private:
  bool isPressed;
};

#endif
