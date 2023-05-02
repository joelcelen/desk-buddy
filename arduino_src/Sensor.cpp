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
  return value;
}

uint16_t Sensor::getIndicatorColor() {
  return indicatorColor;
}

void Sensor::setValue(const char* value) {
  this->value = value;
}

void Sensor::setIndicatorColor(uint16_t indicatorColor) {
  this->indicatorColor = indicatorColor;
}