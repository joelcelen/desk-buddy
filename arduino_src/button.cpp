#include "Button.h"

//Button class
Button::Button() {
  this->isPressed = false; //Set boolean isPressed to false
}

//Method that delays action indefinitely until button 5 is pressed on the Wio terminal.
void Button::delayUntilPressed() {
  while(!isPressed){
  
  if(digitalRead(WIO_5S_PRESS) == LOW){ //If button 5 is pressed change boolean isPressed to true.
    isPressed = true;
  }
}
isPressed = false;
}

