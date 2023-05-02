/**************************************************************
* "Sensor.h" - a library for defining Sensor objects that can be used to represent a sensor
*  and its current value, with an optional indicator color which can be used for TFT displays.
 **************************************************************/

#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h>   // TFT library colors require unsigned 16-bit integers (such as green: 0x07E0)

class Sensor {
  private:
    const char* value;
    uint16_t indicatorColor;

  public:
    //constructors
    Sensor();
    Sensor(uint16_t indicatorColor);
    
    // getters
    const char* getValue();
    uint16_t getIndicatorColor();

    // setters
    void setValue(const char* value);
    void setIndicatorColor(uint16_t indicatorColor);
};

#endif