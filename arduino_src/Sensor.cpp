#include "Sensor.h"

// default constructor
Sensor::Sensor() {
  value = "";
  indicatorColor = 0;
}

Sensor::Sensor(uint16_t startProgramColor) {
  value = "";
  indicatorColor = startProgramColor;
}

const char* Sensor::getValue() {
  return value.c_str();
}

uint16_t Sensor::getIndicatorColor() {
  return indicatorColor;
}

void Sensor::setValue(const char* newValue) {
  value = String(newValue);
}

void Sensor::setIndicatorColor(uint16_t indicatorColor) {
  if (indicatorColor == 0x07E0 || indicatorColor == 0xFDA0 || indicatorColor == 0xF800) {   //primitive exception handling to check for unexpected colors
    this->indicatorColor = indicatorColor;
  }else{
    Serial.println("Runtime exception while setting indicatorColor");                       //statement to help debug
  }
}