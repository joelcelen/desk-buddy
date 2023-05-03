#ifndef _BUTTONTESWIO_H_
#define _BUTTONTESWIO_H_

#include <SPI.h>
#include <TFT_eSPI.h>

//Button class with initialization of methods and variables, private and public where appropriate.
class button {
public:
  button();
  void delayUntilPressed();
private:
  bool isPressed;
};

#endif
