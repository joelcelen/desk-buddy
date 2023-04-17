#include "DHT.h" // DHT11 humidity/temperature sensor
#include "TFT_eSPI.h" // Wio terminal screen

// Create an instance of DHT
DHT dht(A0, DHT11);



// initialize TFT LCD
TFT_eSPI screen;

void setup() {
  
  Serial.begin(9600);
  Wire.begin();
}

void loop() {
  
  
  // Humidity sensor DT11 -----------------------------------

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  // Printing the results in the Serial monitor
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C");

  // Light sensor--------------------------------------------

  lightValue = analogRead(lightPin);  // Read the analog value from the light sensor

  // Printing the results in the Serial monitor  
  Serial.print("Light sensor value: ");  // Print the sensor value to the serial monitor
  Serial.println(lightValue);  

  //--------------------------------------------------------------

  delay(1000);
}
