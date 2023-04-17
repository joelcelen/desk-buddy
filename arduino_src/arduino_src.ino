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
  Wire.begin();
}

void loop() {
  
  
  // Humidity sensor DT11 -----------------------------------

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  // Printing the results in the serial monitor
  serial.print("Humidity: ");
  serial.print(humidity);
  serial.print(" %\t");
  serial.print("Temperature: ");
  serial.print(temperature);
  serial.println(" *C");
  
  //--------------------------------------------------------------

  delay(1000);
}
