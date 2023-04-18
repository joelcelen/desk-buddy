#ifndef _BUTTONTESWIO_H_
#define _BUTTONTESWIO_H_

#include <SPI.h>
#include <TFT_eSPI.h>

class button {
public:
  button();
  void begin();
  void checkButton();
private:
  TFT_eSPI _tft;
  bool _isPressed;
  bool _prevButtonState;
};

#endif
