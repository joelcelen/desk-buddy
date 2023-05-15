/**************************************************************
 * "Button.h" - a simple library for checking button press on Wio 5-way switch, and
 delaying execution until button is pressed (note: delay method not used in current main)
 **************************************************************/

#include "Button.h"

//Button class
Button::Button() {
  this->isPressed = false;                  // Set boolean isPressed to false
}

//Method that delays action indefinitely until button 5 is pressed on the Wio terminal. (Blocking delay.)
void Button::delayUntilPressed() {
  while(!isPressed){
  
  if(digitalRead(WIO_5S_PRESS) == LOW){     // If button 5S is pressed change boolean isPressed to true.
    isPressed = true;
  }
}
isPressed = false;
}

//Method that checks if the button is pressed
bool Button::checkState(){
  if(digitalRead(WIO_5S_PRESS) == LOW){
    return true;
  }else{
    return false;
  }
}
