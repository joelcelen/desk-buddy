#include "button.h"

button::button() : _tft(TFT_eSPI()) {}

void button::begin() {
  _tft.begin();
  _tft.setRotation(1);
  _tft.fillScreen(TFT_NAVY);
  _tft.setTextSize(3);
  _tft.setCursor(70, 70);
  _tft.setTextColor(TFT_PURPLE);
  _tft.println("You've been sitting for too long! Stand up and stretch! Press button to confirm");
  _isPressed = false;
  _prevButtonState = false;
}

void button::checkButton() {
  bool buttonState = digitalRead(BUTTON_1);

  if (buttonState == LOW && _prevButtonState == HIGH) {
    _isPressed = !_isPressed;
    if (_isPressed) {

            
      //if pressed, do something, connect to behavior, e.g. when button clicked-turn off notification for example

      //for showcase only
      _tft.fillScreen(TFT_WHITE);
      _tft.setTextColor(TFT_GREEN);
      _tft.setTextSize(5);
      _tft.setCursor(70, 70);
      _tft.println("Good Job!");
    } else {
      _tft.fillScreen(TFT_BLACK);
      _tft.setTextColor(TFT_RED);
      _tft.setTextSize(5);
      _tft.setCursor(70, 70);
      _tft.println("off");
    }
  }

  _prevButtonState = buttonState;
}

