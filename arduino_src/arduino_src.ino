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

// PRIVATE MQTT broker settings (such as HiveMQ, Mosquitto) - imported from brokerInfo.h file
const char* mqtt_broker = BROKER_URL; // MQTT broker URL
const int mqtt_port = BROKER_PORT; // MQTT broker port
const char* mqtt_client = CLIENT_ID; // MQTT client ID (unique identifer name)
const char* mqtt_username = CLIENT_USERNAME; // MQTT client username - from MQTT broker
const char* mqtt_password = CLIENT_PASSWORD; // MQTT client password - from MQTT broker
const char* rootCACertificate = ROOT_CA_CERTIFICATE; //SSL certificate - from MQTT broker

// MQTT publish (TOPIC_PUB*) and subscribe (TOPIC_SUB*) topics - imported from mqttTopics.h file
const char* mqtt_sub_deskBuddy = TOPIC_SUB; // subscribe to deskBuddy topic
const char* mqtt_pub_deskBuddy = TOPIC_PUB; // publish to deskBuddy topic
const char* mqtt_sub_mood = TOPIC_SUB_MOOD; // subscribe to mood setting
const char* mqtt_sub_pref = TOPIC_SUB_PREF; //subscribe to preference setting
const char* mqtt_sub_sound = TOPIC_SUB_SOUND; //subscribe to sound byte
const char* mqtt_pub_temp = TOPIC_PUB_TEMP; // publish temperature reading
const char* mqtt_pub_humid = TOPIC_PUB_HUMID; // publish humidity reading
const char* mqtt_pub_light = TOPIC_PUB_LIGHT; // publish light reading

WiFiClientSecure wifiSSLClient; //wifi client for PubSub with SSL/TLS cryptographic protocols
PubSubClient mqttClient(wifiSSLClient); //MQTT client with SSL/TLS communication protocols
long lastMsg = 0; //last message time
long lastDisp = 0; //last display feedback on environment preferences
long lastStandUp = 0; //last stand up message
long lastMotivate = 0; //last motivational quote
uint16_t tempColor = TFT_GREEN;
uint16_t humidColor = TFT_ORANGE;
uint16_t lightColor = TFT_RED;

// Create Wi-Fi Instance with SSID & Password 
WIFI myWifi(SSID, PASSWORD);

// Create an instance of DHT
DHT dht(A0, DHT11);

//Create instance of button class named "button"
//button button;

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
  wifiSSLClient.setCACert(rootCACertificate); // Set root CA certificate
  mqttClient.setServer(mqtt_broker, mqtt_port);
  mqttClient.setCallback(mqttCallback);

  //button
  pinMode(WIO_KEY_A, INPUT_PULLUP); //Sets the mode of "BUTTON_1" pin to input with an internal pull-up resistor.
  //button.begin();

  //buzzer
  pinMode(WIO_BUZZER, OUTPUT);

  //tft
  tft.begin();
  tft.setRotation(3); // Set the display rotation to 270 degrees
  tft.fillScreen(TFT_BLACK);
}

void loop() {
  //mqtt_loop - checks for connection to MQTT broker, listens to topic deskBuddy for now
  if (!mqttClient.connected()) {
    mqttReconnect();
  }

  //Button --------------------------------------------------  
  //button.checkButton();

  // Humidity sensor DT11 -----------------------------------

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  // Printing the results in the Serial monitor
  //Serial.print("Humidity: ");
  //Serial.print(humidity);
  //Serial.print(" %\t");
  //Serial.print("Temperature: ");
  //Serial.print(temperature);
  //Serial.println(" *C");

  // Light sensor--------------------------------------------

  int lightValue = analogRead(A2);  // Read the analog value from the light sensor, assigned to pin A2

  //Printing the results in the Serial monitor  
  //Serial.print("Light sensor value: ");  // Print the sensor value to the serial monitor
  //Serial.println(lightValue);  

  //--------------------------------------------------------------
  
  // convert readings to strings
  char tempStr[8], humidStr[8], lightStr[8];
  dtostrf(temperature, 3, 2, tempStr);
  dtostrf(humidity, 3, 2, humidStr);
  itoa(lightValue, lightStr, 4);
  
  // display readings
  //drawLayout(tempStr, humidStr, lightStr, tempColor, humidColor, lightColor);

  

  // Event timer
  long now = millis();
  
  if (now - lastStandUp > 60000) { //300000
    lastStandUp = now;
    drawStandUpMsg();
    buzz();
    //wait for button click to transition from Msg
    //bool buttonPressed = false;
    //while (!buttonPressed) {
    //if (digitalRead(WIO_KEY_A) == LOW) {
    //  buttonPressed = true;
    //}
    delay(2000);
  }else if(now - lastMotivate > 30000){ //150000
    lastMotivate = now;
    
    delay(3000);
    drawMotivationalMsg();
    // msg displays for 30 seconds
    delay(2000);

  }else if (now - lastMsg > 5000) { //30000
    lastMsg = now;

  // publish readings
  mqttClient.publish("deskBuddy/temperature",tempStr);
  mqttClient.publish("deskBuddy/humidity",humidStr);
  mqttClient.publish("deskBuddy/light",lightStr);

  }else if(now - lastDisp > 3000) { // 5000
    lastDisp = now;
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

  if (strcmp(topic, "deskBuddy/app/setTemperature") == 0) {
    if (message == "TFT_GREEN") {
      tempColor = TFT_GREEN;
    } else if (message == "TFT_ORANGE") {
      tempColor = TFT_ORANGE;
    } else if (message == "TFT_RED") {
      tempColor = TFT_RED;
    }
  } else if (strcmp(topic, "deskBuddy/app/setHumid") == 0) {
    if (message == "TFT_GREEN") {
      humidColor = TFT_GREEN;
    } else if (message == "TFT_ORANGE") {
      humidColor = TFT_ORANGE;
    } else if (message == "TFT_RED") {
      humidColor = TFT_RED;
    }
  } else if (strcmp(topic, "deskBuddy/app/setLight") == 0) {
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
    if (mqttClient.connect(mqtt_client, mqtt_username, mqtt_password)) {
      // Once connected, publish an announcement...
      Serial.println("Connected to MQTT broker!");
      mqttClient.publish(mqtt_pub_deskBuddy, "{\"message\": \"Wio Terminal is connected!\"}");
      Serial.println("Published connection message successfully!");
      // ... and resubscribe
      Serial.print("Subcribed to: ");
      Serial.println("deskBuddy");
      mqttClient.subscribe("deskBuddy");
      mqttClient.subscribe("deskBuddy/app/setTemperature");
      mqttClient.subscribe("deskBuddy/app/setHumid");
      mqttClient.subscribe("deskBuddy/app/setLight");
      //mqttClient.subscribe(mqtt_pub_light);
      //mqttClient.subscribe(mqtt_pub_humid);
      //mqttClient.subscribe(mqtt_pub_temp);
    } else {
      Serial.println("Connection to MQTT broker failed!");
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void drawLayout(String tempStr, String humidStr, String lightStr, uint16_t tempColor, uint16_t humidColor, uint16_t lightColor){

  // Clear screen
  tft.fillScreen(TFT_BLACK);

  // Set text properties
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);

  // Draw temperature box and text
  tft.fillRect(10, 15, 20, 20, tempColor);
  tft.setTextFont(2); // Select custom font
  tft.drawString("Temperature", 40, 10);
  tft.setTextFont(4); // Select system font
  tft.drawString(tempStr + " C", 40, 40);

  // Draw humidity box and text
  tft.fillRect(10, 90, 20, 20, humidColor);
  tft.setTextFont(2); // Select custom font
  tft.drawString("Humidity", 40, 85);
  tft.setTextFont(4); // Select system font
  tft.drawString(humidStr + " %", 40, 115);

  // Draw light level box and text
  tft.fillRect(10, 165, 20, 20, lightColor);
  tft.setTextFont(2); // Select custom font
  tft.drawString("Light Level", 40, 160);
  tft.setTextFont(4); // Select system font
  tft.drawString(lightStr + " lx", 40, 190);

  //delay(1000); // Wait for 1 second before updating display
}

void drawStandUpMsg(){
  // Clear screen
  tft.fillScreen(TFT_BLACK);

  // Set text properties
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);

  // Draw message for standing up and stretching
  tft.setTextFont(1); // Select system font
  tft.setCursor(30, 50); //try 10, 250 also
  tft.println("You have been sitting");
  tft.setCursor(30, 110); //try 10, 250 also
  tft.println(" for too long!! o_o");
  tft.setCursor(30, 170); //try 10, 250 also
  tft.println("Stand up and stretch!");
}

void drawMotivationalMsg() {

  // Clear screen
  tft.fillScreen(TFT_BLACK);

  // Set text properties
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);

  // Draw motivational message
  tft.setTextFont(1); // Select system font
  tft.setCursor(30, 50); //try 10, 250 also
  tft.println("Keep going!");
  tft.setCursor(30, 110); //try 10, 250 also
  tft.println("You can do it!!");
  tft.setCursor(30, 170); //try 10, 250 also
  tft.println("You're doing great!!!");
}

void buzz(){
  analogWrite(WIO_BUZZER, 128);
  delay(1000);
  analogWrite(WIO_BUZZER, 0);
  delay(1000);
}