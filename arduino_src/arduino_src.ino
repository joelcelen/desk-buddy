#include "WIFI.h" //Wi-Fi Custom library
#include <rpcWiFi.h> //Wi-Fi System library
#include "networkInfo.h" //contains wifi SSID and PASSWORD

#include "DHT.h" // DHT11 humidity/temperature sensor
#include "TFT_eSPI.h" // Wio terminal screen
#include "button.h" //Reference to the header file for button

//MQTT libraries
#include <PubSubClient.h> // MQTT library
#include <WiFiClientSecure.h> // secure Wifi client for PubSub library
#include "brokerInfo.h" //secrets file for mqtt broker (.gitignore)
#include "mqttTopics.h" //secrets file for mqtt broker topics (.gitignore)

WiFiClientSecure wifiSSLClient; //wifi client for PubSub with SSL/TLS cryptographic protocols
PubSubClient mqttClient(wifiSSLClient); //MQTT client with SSL/TLS communication protocols
long lastPublish = 0; //last message time
long lastDisplay = 0; //last display time feedback on environment preferences
long lastStandUp = 0; //last stand up message time
long lastMotivate = 0; //last motivational quote time
// initialize TFT colors for UI based on Android App environment preferences
uint16_t tempColor = TFT_GREEN;
uint16_t humidColor = TFT_ORANGE;
uint16_t lightColor = TFT_RED;

// Create Wi-Fi Instance with SSID & Password 
WIFI myWifi(SSID, PASSWORD);

// Create an instance of DHT
DHT dht(A0, DHT11);

//Create instance of button class named "button"
button button;

// initialize TFT LCD
TFT_eSPI tft;

void setup() {
  
  Serial.begin(115200);
  while (!Serial);
  
  //initialize Wi-Fi
  Serial.print("Attempting to connect to Wi-Fi address: ");
  Serial.println(myWifi.getSSID()); // Use getter function to get SSID
  myWifi.connect(); // Method to initialize Wi-Fi connection
   
  Wire.begin();

  //mqtt_setup - connects to secure MQTT broker
  wifiSSLClient.setCACert(ROOT_CA_CERTIFICATE); // Set root CA certificate
  mqttClient.setServer(BROKER_URL, BROKER_PORT);
  mqttClient.setCallback(mqttCallback);

  //button
  pinMode(WIO_5S_PRESS, INPUT_PULLUP); //Sets the mode of "Wio button 5 press" pin to input with an internal pull-up resistor.
  //button.begin();

  //buzzer
  pinMode(WIO_BUZZER, OUTPUT);

  //tft
  tft.begin();
  tft.setRotation(3); // Set the display rotation to 270 degrees
  tft.fillScreen(TFT_BLACK);
}

void loop() {
  //Connect MQTT - Verify connection
  if (!mqttClient.connected()) {
    mqttReconnect();
  }
  

  // Humidity sensor DT11 -----------------------------------

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Light sensor--------------------------------------------

  int lightValue = analogRead(A2);  // Read the analog value from the light sensor, assigned to pin A2
  
  // convert readings to strings
  char tempStr[8], humidStr[8], lightStr[8];
  dtostrf(temperature, 3, 2, tempStr);
  dtostrf(humidity, 3, 2, humidStr);
  itoa(lightValue, lightStr, 10);

  // Event timer
  long now = millis();


  
  // Control TFT scenes based on event timer
  if (now - lastStandUp > 60000) { //300000
    lastStandUp = now;
    drawStandUpMsg(); //Draws screen with prompt to stand up and stretch
    buzz(); //Makes noise to alert user
    button.delayUntilPressed(); //Cause the program to delay indefinitely until button 5 is pressed.
    Serial.println("Good Job!");
    delay(2000);
  }else if(now - lastMotivate > 30000){ //150000 ms better for final product
    lastMotivate = now;
    
    delay(3000);
    drawMotivationalMsg();
    // msg displays for 30 seconds
    delay(2000);

  }else if (now - lastPublish > 5000) { //30000 ms better for final product
    lastPublish = now;

  // publish readings
  mqttClient.publish(TOPIC_PUB_TEMP,tempStr);
  mqttClient.publish(TOPIC_PUB_TEMP,humidStr);
  mqttClient.publish(TOPIC_PUB_TEMP,lightStr);

  }else if(now - lastDisplay > 3000) { // 5000 ms better for final product
    lastDisplay = now;
    // update feedback from app on environmental preference targets
    drawLayout(tempStr, humidStr, lightStr, tempColor, humidColor, lightColor);
  }
  
  mqttClient.loop(); // Receive messages on subscribed topics and trigger mqttCallback function
  delay(1000);
}

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  String message = "";
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
    message += (char)payload[i];
  }
  Serial.println();

  if (strcmp(topic, TOPIC_SUB_TEMP) == 0) {
    if (message == "TFT_GREEN") {
      tempColor = TFT_GREEN;
    } else if (message == "TFT_ORANGE") {
      tempColor = TFT_ORANGE;
    } else if (message == "TFT_RED") {
      tempColor = TFT_RED;
    }
  } else if (strcmp(topic, TOPIC_SUB_HUMID) == 0) {
    if (message == "TFT_GREEN") {
      humidColor = TFT_GREEN;
    } else if (message == "TFT_ORANGE") {
      humidColor = TFT_ORANGE;
    } else if (message == "TFT_RED") {
      humidColor = TFT_RED;
    }
  } else if (strcmp(topic, TOPIC_SUB_LIGHT) == 0) {
    if (message == "TFT_GREEN") {
      lightColor = TFT_GREEN;
    } else if (message == "TFT_ORANGE") {
      lightColor = TFT_ORANGE;
    } else if (message == "TFT_RED") {
      lightColor = TFT_RED;
    }
}
}

void mqttReconnect()
{
  // Loop until we're reconnected
  while (!mqttClient.connected())
  {
    Serial.println("Connecting to MQTT broker...");
    // Attempt to connect
    if (mqttClient.connect(CLIENT_ID, CLIENT_USERNAME, CLIENT_PASSWORD)) {
      // Once connected, publish an announcement...
      Serial.println("Connected to MQTT broker!");
      mqttClient.publish(TOPIC_PUB, "{\"message\": \"Wio Terminal is connected!\"}");
      Serial.println("Published connection message successfully!");

      // Subscribe to topics that App publishes to, to update UI based on user preferences
      mqttClient.subscribe(TOPIC_SUB_TEMP);
      mqttClient.subscribe(TOPIC_SUB_HUMID);
      mqttClient.subscribe(TOPIC_SUB_LIGHT);

    } else {
      //if not connected:
      Serial.println("Connection to MQTT broker failed!");
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// REFACTOR TO UI.h
void drawLayout(String tempStr, String humidStr, String lightStr, uint16_t tempColor, uint16_t humidColor, uint16_t lightColor){

  // Clear screen
  tft.fillScreen(TFT_BLACK);

  // Set text properties
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);

  // Draw temperature box and text
  tft.fillRect(10, 15, 20, 20, tempColor);
  tft.setTextFont(2);
  tft.drawString("Temperature", 40, 10);
  tft.setTextFont(4);
  tft.drawString(tempStr + " C", 40, 40);

  // Draw humidity box and text
  tft.fillRect(10, 90, 20, 20, humidColor);
  tft.setTextFont(2);
  tft.drawString("Humidity", 40, 85);
  tft.setTextFont(4);
  tft.drawString(humidStr + " %", 40, 115);

  // Draw light level box and text
  tft.fillRect(10, 165, 20, 20, lightColor);
  tft.setTextFont(2);
  tft.drawString("Light Level", 40, 160);
  tft.setTextFont(4);
  tft.drawString(lightStr + " lx", 40, 190);
}

void drawStandUpMsg(){
  // Clear screen
  tft.fillScreen(TFT_BLACK);

  // Set text properties
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);

  // Draw message for standing up and stretching
  tft.setTextFont(1);
  tft.setCursor(30, 50);
  tft.println("You have been sitting");
  tft.setCursor(30, 110);
  tft.println(" for too long!! o_o");
  tft.setCursor(30, 170);
  tft.println("Stand up and stretch!");
}

void drawMotivationalMsg() {

  // Clear screen
  tft.fillScreen(TFT_BLACK);

  // Set text properties
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);

  // Draw motivational message
  tft.setTextFont(1);
  tft.setCursor(30, 50);
  tft.println("Keep going!");
  tft.setCursor(30, 110);
  tft.println("You can do it!!");
  tft.setCursor(30, 170);
  tft.println("You're doing great!!!");
}

// Refactor to Buzzer.h
void buzz(){
  analogWrite(WIO_BUZZER, 128);
  delay(1000);
  analogWrite(WIO_BUZZER, 0);
  delay(1000);
}