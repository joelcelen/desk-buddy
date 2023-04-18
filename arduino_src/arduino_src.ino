#include "DHT.h" // DHT11 humidity/temperature sensor
#include "TFT_eSPI.h" // Wio terminal screen
#include "button.h" //Reference to the header file for button

// Create an instance of DHT
DHT dht(A0, DHT11);

//Create instance of button class named "button"
button button;

// initialize TFT LCD
TFT_eSPI screen;

void setup() {
  
  Serial.begin(9600);
  Wire.begin();

  pinMode(BUTTON_1, INPUT_PULLUP); //Sets the mode of "BUTTON_1" pin to input with an internal pull-up resistor.
  button.begin();
}

void loop() {

  //Button --------------------------------------------------  
  
  button.checkButton();

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

  int lightValue = analogRead(A2);  // Read the analog value from the light sensor, assigned to pin A2

  //Printing the results in the Serial monitor  
  Serial.print("Light sensor value: ");  // Print the sensor value to the serial monitor
  Serial.println(lightValue);  

  //--------------------------------------------------------------
  
  delay(100);
}
