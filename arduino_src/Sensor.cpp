#include "Sensor.h"

// default constructor with black indicator color
Sensor::Sensor(String defaultPrefValue, float defaultPrefGreen, float defaultPrefOrange) {
  value = "";
  prefValue = String(defaultPrefValue);
  prefGreen = defaultPrefGreen;
  prefOrange = defaultPrefOrange;
  indicatorColor = 0;                                                  // default indicator color is black
}

const char* Sensor::getValue() {
  return value.c_str();
}

uint16_t Sensor::getIndicatorColor() {
  return indicatorColor;
}

void Sensor::setValue(const char* newValue) {
  value = String(newValue);
  updateIndicatorColor();
}

void Sensor::setPrefValue(const char* newPrefValue){
  prefValue = String(newPrefValue);
  updateIndicatorColor();
}

void Sensor::updateIndicatorColor() {
  float floatValue = atof(value.c_str());
  float floatPrefValue = atof(prefValue.c_str());

  if (abs(floatValue - floatPrefValue)  <= prefGreen) {                // update indicator based on deviation from preferences
    indicatorColor = 0x07E0;                                           // value of TFT_GREEN in TFT_eSPI library
  } else if (abs(floatValue - floatPrefValue) <= prefOrange) {
    indicatorColor = 0xFDA0;                                           // value of TFT_ORANGE in TFT_eSPI library
  } else {
    indicatorColor = 0xF800;                                           // value of TFT_RED in TFT_eSPI library
  }
}