/**************************************************************
* "Sensor.h" - a library for defining Sensor objects that can be used to represent a sensor
*  and its current value, with an optional indicator color which can be used for TFT displays.
 **************************************************************/

#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h>              // TFT library colors require unsigned 16-bit integers (such as green: 0x07E0)
#include <Arduino.h>             // need this library to include Strings

class Sensor {
  private:
    String value;                // String value of sensor reading to publish by MQTT (String allows easier reuse across different sensors that measure in float, int, long, etc.)
    String prefValue;            // String value of optimal sensor reading as as defined by user for their comfort
    float prefGreen;             // absolute value difference from prefValue to show green indicator
    float prefOrange;            // absolute value difference from prefValue to show orange indicator (displays red outside this range)
    uint16_t indicatorColor;     // indicator color for TFT display
    
    // update indicator color
    void updateIndicatorColor();

  public:
    // constructors
    Sensor(String defaultPrefValue, float defaultPrefGreen, float defaultPrefOrange);
    
    // getters
    const char* getValue();
    uint16_t getIndicatorColor();

    // setters
    void setValue(const char* newValue);
    void setPrefValue(const char* newPrefValue);
    void setPrefGreen(float newGreenValue);
    void setPrefOrange(float newOrangeValue);
};

#endif