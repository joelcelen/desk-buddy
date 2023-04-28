#ifndef BUTTON_H
#define BUTTON_H

#include <SPI.h>

//Button class with initialization of methods and variables, private and public where appropriate.
class button {
public:
  button();
  void delayUntilPressed();
private:
  bool isPressed;
};

#endif
