//WiFi libraries
#include "WIFI.h"                             //Wi-Fi Custom library
#include <rpcWiFi.h>                          //Wi-Fi System library
#include "networkInfo.h"                      //contains wifi SSID and PASSWORD

//MQTT libraries
#include <PubSubClient.h>                     // MQTT library
#include <WiFiClientSecure.h>                 // secure Wifi client for PubSub library
#include "brokerInfo.h"                       //secrets file for mqtt broker (.gitignore)
#include "mqttTopics.h"                       //secrets file for mqtt broker topics (.gitignore)

// Sensor libraries
#include "DHT.h"                              //DHT sensor library (temperature and humidity)    

// Display libraries
#include "TFT_eSPI.h"                         // TFT screen library

WIFI myWifi(SSID, PASSWORD);                  //Create Wi-Fi Instance with SSID & Password

WiFiClientSecure wifiSSLClient;               //wifi client for PubSub with SSL/TLS cryptographic protocols
PubSubClient mqttClient(wifiSSLClient);       //MQTT client with SSL/TLS communication protocols
String message = "";                          //MQTT default message (empty string)

DHT dht(A0, DHT11);                           // Create an instance of DHT (temperature and humidity) sensor

// initializing sensor measurement variables and their string conversions
float temperature;
float humidity;
int lightValue;
char tempStr[8], humidStr[8], lightStr[8];

// button
bool buttonPressed = false;                   // button class/variables

// Event timer
unsigned long timerStart;
unsigned long timerEnd;
unsigned long now;

// GUI update intervals are intentionally short for demo purposes
unsigned long intervalStandUp = 71000;       // time interval to display stand up messages
unsigned long intervalMotivate = 43000;      // time interval to display motivational messages
unsigned long intervalPublish = 3000;        // time interval to publish sensor data
unsigned long intervalDisplay = 5000;        // time interval to refresh dashboard display
unsigned long lastPublish = 0;               //last MQTT publish time of sensor data
unsigned long lastDisplay = 0;               //last update of indicators based on app environment preferences
unsigned long lastStandUp = 0;               //last stand up message time
unsigned long lastMotivate = 0;              //last motivational quote time

// initialize TFT LCD
TFT_eSPI tft;

// default TFT indicator colors for dashboard
uint16_t tempColor = TFT_GREEN;              //default indicator color for temperature
uint16_t humidColor = TFT_ORANGE;            //default indicator color for humidity
uint16_t lightColor = TFT_RED;               //default indicator color for light level

String deskBuddyLogo = "      _           _    ____            _     _       \n"
                       "     | |         | |  |  _ \\          | |   | |      \n"
                       "   __| | ___  ___| | _| |_) |_   _  __| | __| |_   _ \n"
                       "  / _` |/ _ \\/ __| |/ /  _ <| | | |/ _` |/ _` | | | |\n"
                       " | (_| |  __/\\__ \\   <| |_) | |_| | (_| | (_| | |_| |\n"
                       "  \\__,_|\\___||___/_|\\_\\____/ \\__,_|\\__,_|\\__,_|\\__, |\n"
                       "                                                __/ |\n"
                       "                                               |___/ ";


void setup() {
  //display GUI when you power on device
  tft.begin();
  drawLaunchScreen();

  //setup Serial communication
  Serial.begin(115200);
  while (!Serial); //wait for opening serial port

  //setup Wi-Fi connection -- based on networkInfo.h parameters
  drawConnectingToWifi();
  myWifi.connect();  // Method to initialize Wi-Fi connection
  tft.println("connected!");
  delay(100);

  //setup MQTT connection -- based on brokerInfo.h parameters
  wifiSSLClient.setCACert(ROOT_CA_CERTIFICATE);  // Set root CA certificate for SSL/TLS encryption
  mqttClient.setServer(BROKER_URL, BROKER_PORT); // Set MQTT broker URL and port
  mqttClient.setCallback(mqttCallback);          // Set callback method -- what to do when a message is received
  tft.setCursor(20, 30);
  tft.print("Connecting to MQTT broker... ");
  mqttConnect();                                 // connect to MQTT (also verify wifi connection)
  delay(100);
  tft.println("connected!");                     // connected message
  tft.setCursor(20, 40);
  delay(100);
  tft.println("Preferences updated!");           // we will update user preferences once the loop starts, but display it now

  //setup button
  pinMode(WIO_5S_PRESS, INPUT_PULLUP);

  //setup buzzer
  pinMode(WIO_BUZZER, OUTPUT);

  tft.setCursor(0, 100);
  tft.print(deskBuddyLogo);                      // display desk buddy logo
  delay(1500);
  drawAuthorsMsg();                              // display list of authors of project
  delay(900);
}

void loop() {
  timerStart = millis();                         //start timing the loop for debugging purposes -- takes about 500-600ms to get through a loop right now!

  //Connect MQTT - Verify connection
  mqttConnect();                                 // verify MQTT connection
  mqttClient.loop();                             // Retrieve messages on subscribed topics and trigger mqttCallback function

  // read sensors data
  temperature = dht.readTemperature();           //read from temperature sensor
  humidity = dht.readHumidity();                 //read from humidity sensor
  lightValue = analogRead(A2);                   // read from light sensor -- which should be connected to pin A2

  //start event timer
  now = millis();

  // Control GUI/TFT scenes based on event timer
  if (now - lastStandUp > intervalStandUp) {
    drawStandUpMsg();                            // display standup messsage, user will be prompted to click button
    buzz();                                      // buzz notification
    waitForButtonClick();                        // wait until button is clicked
    drawGoodJobMsg();                            // encouraging message (positive reinforcement)
    delay(2000);
    lastStandUp = millis();

  } else if (now - lastMotivate > intervalMotivate) {
    drawMotivationalMsg();                       // draw motivational messages
    delay(2500);
    lastMotivate = millis();

  } else if (now - lastPublish > intervalPublish) {
    mqttPublishSensorData();                     //publish sensor data
    lastPublish = millis();

  } else if (now - lastDisplay > intervalDisplay) {
    //update dashboard display with sensor data, and indicators that change color according to user preferences
    drawDashboard(tempStr, humidStr, lightStr, tempColor, humidColor, lightColor);
    lastDisplay = millis();
  }
  timerEnd = millis();                          //end timing the loop for debugging
  Serial.println((timerEnd - timerStart));      //print serial port msg for debugging
}

//wait for button click to transition from StandUpMsg
void waitForButtonClick() {
  while (!buttonPressed) {
    if (digitalRead(WIO_5S_PRESS) == LOW) {
      buttonPressed = true;
    }
  }
  buttonPressed = false;
}

// buzzer method -- buzzes for 1 second to notify user
void buzz() {
  analogWrite(WIO_BUZZER, 128);
  delay(1000);
  analogWrite(WIO_BUZZER, 0);
}

// parser method to parse incoming MQTT messages (payloads) based on subscription topic
void parser(char* topic) {
  if (strcmp(topic, TOPIC_SUB_TEMP) == 0) {
    // Temperature topic parser
    if (message == "TFT_GREEN") {
      tempColor = TFT_GREEN;
    } else if (message == "TFT_ORANGE") {
      tempColor = TFT_ORANGE;
    } else if (message == "TFT_RED") {
      tempColor = TFT_RED;
    }
  } else if (strcmp(topic, TOPIC_SUB_HUMID) == 0) {
    // Humidity topic parser
    if (message == "TFT_GREEN") {
      humidColor = TFT_GREEN;
    } else if (message == "TFT_ORANGE") {
      humidColor = TFT_ORANGE;
    } else if (message == "TFT_RED") {
      humidColor = TFT_RED;
    }
  } else if (strcmp(topic, TOPIC_SUB_LIGHT) == 0) {
    // Light level topic parser
    if (message == "TFT_GREEN") {
      lightColor = TFT_GREEN;
    } else if (message == "TFT_ORANGE") {
      lightColor = TFT_ORANGE;
    } else if (message == "TFT_RED") {
      lightColor = TFT_RED;
    }
  } else if (strcmp(topic, TOPIC_SUB_MOOD) == 0) {
    // Mood topic parser
    // Do nothing for now, feature may be added later in Sprint 3
  } else if (strcmp(topic, TOPIC_SUB_TIMING) == 0) {
    // Timing topic parser
    // Do nothing for now, feature may be added later in Sprint 3
  } else if (strcmp(topic, TOPIC_SUB_MOTIVATION) == 0) {
    // Motivational messages topic parser
    // Do nothing for now, feature may be added later in Sprint 3
  }
}

void mqttPublishSensorData() {
  // convert sensor measurements to strings
  dtostrf(temperature, 3, 2, tempStr);
  dtostrf(humidity, 3, 2, humidStr);
  itoa(lightValue, lightStr, 10);  //int base 10
  // publish sensor measurements to broker
  mqttClient.publish(TOPIC_PUB_TEMP, tempStr);    //temperaturePublisher();
  mqttClient.publish(TOPIC_PUB_HUMID, humidStr);  //humidityPublisher();
  mqttClient.publish(TOPIC_PUB_LIGHT, lightStr);  //lightPublisher();
}

void mqttSubscribeToAppTopics() {
  mqttClient.subscribe(TOPIC_SUB_TEMP);        //temperatureSubscriber();
  mqttClient.subscribe(TOPIC_SUB_HUMID);       //humiditySubscriber();
  mqttClient.subscribe(TOPIC_SUB_LIGHT);       //lightSubscriber();
  mqttClient.subscribe(TOPIC_SUB_MOOD);        //moodSubscriber();
  mqttClient.subscribe(TOPIC_SUB_TIMING);      //timingSubscriber();
  mqttClient.subscribe(TOPIC_SUB_MOTIVATION);  //motivationSubscriber();
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];              //append to message string based on payload
  }
  parser(topic);                              //call parser method to check topic and parse payload
}

// MQTT connection method
void mqttConnect(){
  if (!mqttClient.connected()) {
    Serial.println("MQTT connection lost.");
    if (!myWifi.isConnected()) {
      Serial.println("WIFI connection lost.");
      myWifi.connect();
    }
    mqttReconnect();
  }
}

// MQTT reconnection method
void mqttReconnect() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT broker...");
    // Attempt to connect to broker
    if (mqttClient.connect(CLIENT_ID, CLIENT_USERNAME, CLIENT_PASSWORD)) {
      // Once connected, publish an announcement to broker...
      Serial.println("Connected to MQTT broker!");
      mqttClient.publish(TOPIC_PUBSUB, "{\"message\": \"Wio Terminal is connected!\"}");
      
      // Confirm the message was sent on serial port
      Serial.println("Published connection message successfully!");

      // Subscribe to topics that App publishes to, to update UI based on user preferences
      mqttSubscribeToAppTopics();

    } else {
      //if not connected:
      Serial.println("... connection to MQTT broker failed! ");
      Serial.print("rc= ");
      Serial.print(mqttClient.state()); // display error message from PubSubClient library
      Serial.println(". Trying again in 5 seconds");
      delay(5000); // Wait 5 seconds before retrying
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////
// GUI methods are listed below here:
void drawLaunchScreen(){
  tft.setRotation(3);  // Set the display rotation to 270 degrees
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setTextFont(1);
  tft.println("");
  tft.setCursor(20, 10);
  tft.println("Launching deskBuddy...");
}

void drawConnectingToWifi(){
  tft.setCursor(20, 20);
  tft.print("Connecting to Wi-Fi network");
  String mySSID = String(myWifi.getSSID());
  if (mySSID.length() > 5) {  // screen doesn't like printing Wi-Fi names more than 5 characters
    tft.print("... ");
  } else {
    tft.print("(");
    tft.print(mySSID);
    tft.print(")... ");
  }
}

void drawDashboard(String tempStr, String humidStr, String lightStr, uint16_t tempColor, uint16_t humidColor, uint16_t lightColor) {
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

void drawStandUpMsg() {
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

  delay(2000);

  // Draw message instructing button click
  tft.setTextSize(1);
  tft.setCursor(290, 220);
  tft.println("press");
  tft.setCursor(294, 230);
  tft.println("(OK)");
}

void drawGoodJobMsg() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(3);
  tft.setTextColor(TFT_WHITE);
  tft.setTextFont(1);
  tft.setCursor(30, 50);
  tft.println("Good Job!");
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
  tft.println("Keep going!");  //refactor these to print published msgs from broker
  tft.setCursor(30, 110);
  tft.println("You can do it!!");
  tft.setCursor(30, 170);
  tft.println("You're doing great!!!");
}

void drawAuthorsMsg() {
  tft.setCursor(10, 190);
  tft.print("Authors:");
  tft.setCursor(70, 200);
  tft.print("Nasit Vurgun");
  tft.setCursor(70, 210);
  tft.print("Rizwan Rafique");
  tft.setCursor(70, 220);
  tft.print("Joel Celen");
  tft.setCursor(180, 200);
  tft.print("Ahmad Haj Ahmad");
  tft.setCursor(180, 210);
  tft.print("Malte Bengtsson");
  tft.setCursor(180, 220);
  tft.print("Karl Eriksson");
  delay(1100);
}