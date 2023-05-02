#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h>   // need this because the TFT library only supports colors that are unsigned 16-bit integers (such as green: 0x07E0)

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