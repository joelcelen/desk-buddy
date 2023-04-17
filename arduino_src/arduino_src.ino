#include "DHT.h" // DHT11 humidity/temperature sensor
#include "TFT_eSPI.h" // Wio terminal screen

// Create an instance of DHT
DHT dht(A0, DHT11);

// initialize TFT LCD
TFT_eSPI screen;

// Define the serial
#define serial Serial

void setup() {
    serial.begin(115200);
    serial.println("DHT11 test!");
    Wire.begin();
}

void loop() {
  
  
  // Humidity sensor DT11 -----------------------------------
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)

  float temp_hum_val[2] = {0};
  
  if (!dht.readTempAndHumidity(temp_hum_val)) {
    serial.print("Humidity: ");
    serial.print(temp_hum_val[0]);
    serial.print(" %\t");
    serial.print("Temperature: ");
    serial.print(temp_hum_val[1]);
    serial.println(" *C");
  } else {
    serial.println("Failed to get temprature and humidity value.");
  }
  
  //--------------------------------------------------------------

  delay(1000);
}
