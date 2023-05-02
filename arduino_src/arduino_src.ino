/**************************************************************
 * Project: Desk Buddy v1.0 (Sprint 2 Demo)
 * Description: A smart desk assistant built with Arduino and connected to the cloud. It helps you monitor your environment and stay productive.
 * 
 * Gitlab Home Page: https://git.chalmers.se/courses/dit113/2023/group-8/desk-buddy/-/wikis/home
 * Authors: Nasit Vurgun, Rizwan Rafique, Joel Celen, Ahmad Haj Ahmad, Malte Bengtsson, Karl Eriksson
 * Contact: gus***na@student.gu.se - replace *** with the first three letters of the first author's last name
 * Date: May 1, 2023
 * 
 * Copyright (c) 2023 Desk Buddy
 * 
 * License: MIT License
 **************************************************************/

//WiFi libraries
#include "WIFI.h"                             //Wi-Fi Custom library
#include <rpcWiFi.h>                          //Wi-Fi System library
#include "networkInfo.h"                      //contains wifi SSID and PASSWORD

//MQTT libraries. See API info here: https://pubsubclient.knolleary.net/api
#include <PubSubClient.h>                     // MQTT library
#include <WiFiClientSecure.h>                 // secure Wifi client for PubSub library
#include "brokerInfo.h"                       //secrets file for mqtt broker (.gitignore)
#include "mqttTopics.h"                       //secrets file for mqtt broker topics (.gitignore)

// Sensor libraries
#include "DHT.h"                              //DHT sensor library (temperature and humidity)    
#include "Button.h"                           //Button class

// Actuator libraries
#include "Buzzer.h"                           //Buzzer class

// Display libraries
//#include "TFT_eSPI.h"                         // TFT screen library
#include "Display.h"                          // Display class

WIFI myWifi(SSID, PASSWORD);                  //Create Wi-Fi Instance with SSID & Password

WiFiClientSecure wifiSSLClient;               //wifi client for PubSub with SSL/TLS cryptographic protocols
PubSubClient mqttClient(wifiSSLClient);       //MQTT client with SSL/TLS communication protocols
String message = "";                          //MQTT default message (empty string)

// initializing sensor measurement variables and their string conversions
DHT dht(A0, DHT11);                           // Create an instance of DHT (temperature and humidity) sensor

float temperature;                            // variable stores temperature reading
float humidity;                               // variable stores humidity reading
int lightValue;                               // variable stores light level reading
char tempStr[8], humidStr[8], lightStr[8];    // variables for converting float or int measurements to string 

Button button;                                // Create an instance of button

// initializing actuators
Buzzer buzzer;                                // Create an instance of buzzer

// Event timer - for loop
unsigned long timerStart;                    // timer for debugging purposes
unsigned long timerEnd;                      // timer for debugging purposes
unsigned long delayStart;                    // timer for custom nonblocking delay method (so MQTT can keep looping)

// GUI update intervals are intentionally short for demo purposes
unsigned long intervalStandUp = 97000;       // time interval to display stand up messages          (default: 97000)
unsigned long intervalMotivate = 43000;      // time interval to display motivational messages      (default: 43000)
unsigned long intervalPublish = 3000;        // time interval to publish sensor data                (default: 3000)
unsigned long intervalDisplay = 5000;        // time interval to refresh dashboard display          (default: 5000)
unsigned long intervalNotification = 3600000;// time interval to send notification to user          (default: 3600000)

// event timers for publish, display, standup, motivate events
unsigned long lastPublish = 0;               //last MQTT publish time of sensor data
unsigned long lastDisplay = 0;               //last update of indicators based on app environment preferences
unsigned long lastStandUp = 0;               //last stand up message time
unsigned long lastMotivate = 0;              //last motivational quote time
unsigned long lastNotification = 0;          //last notification send time

// Count number of times user stands up during session.
int countStandUps = 0;

// Motivational message
String motivationalMessage;

// Notification message
String notificationMessage;

// initialize TFT LCD display
Display display;

// default TFT indicator colors for dashboard
uint16_t tempColor = TFT_GREEN;              //default indicator color for temperature
uint16_t humidColor = TFT_ORANGE;            //default indicator color for humidity
uint16_t lightColor = TFT_RED;               //default indicator color for light level

const char* deskBuddyLogo = "      _           _    ____            _     _       \n"
                       "     | |         | |  |  _ \\          | |   | |      \n"
                       "   __| | ___  ___| | _| |_) |_   _  __| | __| |_   _ \n"
                       "  / _` |/ _ \\/ __| |/ /  _ <| | | |/ _` |/ _` | | | |\n"
                       " | (_| |  __/\\__ \\   <| |_) | |_| | (_| | (_| | |_| |\n"
                       "  \\__,_|\\___||___/_|\\_\\____/ \\__,_|\\__,_|\\__,_|\\__, |\n"
                       "                                                __/ |\n"
                       "                                               |___/ ";
// ASCII text (font: Big, layouts: default). Generated using: https://www.coolgenerator.com/ascii-text-generator
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  display.init();                                     // set up Wio Display
  display.drawLaunchScreen();                            // display GUI when you power on device

  //setup Serial communication
  Serial.begin(115200);                          // open serial port with max bit rate
  //while (!Serial);                             // wait for serial port to open -- useful if debugging things in setup

  display.drawConnectingToWifi(myWifi.getSSID());        // connecting msg
  myWifi.connect();                              // setup Wi-Fi connection -- based on networkInfo.h parameters
  display.drawConnectedToWifi();                         // connected msg
  delay(100);                                    // this blocking delay is for visual aesthetics on startup screen

  //setup MQTT connection -- based on brokerInfo.h parameters
  wifiSSLClient.setCACert(ROOT_CA_CERTIFICATE);  // Set root CA certificate for SSL/TLS encryption
  mqttClient.setServer(BROKER_URL, BROKER_PORT); // Set MQTT broker URL and port
  mqttClient.setCallback(mqttCallback);          // Set callback method -- what to do when a message is received
  display.drawConnectingToMQTT();
  mqttConnect();                                 // connect to MQTT (also verify wifi connection)
  nonBlockingDelay(100);
  display.drawConnectedToMQTT();                         // connected message
  nonBlockingDelay(100);
  display.drawPreferencesUpdated();                      // we will update user preferences once the loop starts, but display it now

  //setup sensors and actuators
  dht.begin();
  pinMode(WIO_5S_PRESS, INPUT_PULLUP);           // setup button sensor
  pinMode(WIO_BUZZER, OUTPUT);                   // setup buzzer actuator

  display.drawDeskBuddyLogo(deskBuddyLogo);                           // display desk buddy logo
  nonBlockingDelay(1500);
  display.drawAuthorsMsg();                              // display list of authors of project
  nonBlockingDelay(2000);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {
  timerStart = millis();                         //start timing the loop for debugging purposes -- takes about 500-600ms to get through a loop right now!

  //Connect MQTT - Verify connection
  mqttConnect();                                 // verify MQTT connection
  mqttClient.loop();                             // Retrieve messages on subscribed topics and trigger mqttCallback function

  // read sensors data
  temperature = dht.readTemperature();           // read from temperature sensor
  humidity = dht.readHumidity();                 // read from humidity sensor
  lightValue = analogRead(A2);                   // read from light sensor -- which should be connected to pin A2

  // Control GUI/TFT scenes based on event timer
  if (millis() - lastStandUp > intervalStandUp) {
    display.drawStandUpMsg();                            // display standup messsage, user will be prompted to click button
    buzzer.notifyLoudly();                       // buzz notification
    nonBlockingDelay(1000);
    display.drawButtonPressMsg();                        // prompts user to press button
    while(!button.checkState()){                 // wait until button is pressed
      nonBlockingDelay(100);
    }
    countStandUps++;
    display.drawGoodJobMsg(countStandUps);               // display good job message (encourages user, positive reinforcement)
    nonBlockingDelay(2000);
    lastStandUp = millis();                      //reset timer

  } else if (millis() - lastMotivate > intervalMotivate) {
    if (motivationalMessage.length() == 0) {
      display.drawMotivationalMsg();                       //draw default motivational message
    }else{
      display.drawMotivationalMsg(motivationalMessage);   //user-defined motivational message
    }
    nonBlockingDelay(2500);
    lastMotivate = millis();                     //reset timer

  } else if (millis() - lastPublish > intervalPublish) {
    mqttPublishSensorData();                     //publish sensor data
    lastPublish = millis();                      //reset timer

  } else if (millis() - lastDisplay > intervalDisplay) {
    display.drawDashboard(tempStr, humidStr, lightStr, tempColor, humidColor, lightColor);  //update dashboard
    lastDisplay = millis();                      //reset timer

  } else if (millis() - lastNotification > intervalNotification) {
    if (notificationMessage.length() == 0) {
      display.drawNotificationMsg();                       //draw default notification msg
    }else{
      display.drawNotificationMsg(notificationMessage);   //user-defined motivational message
    }
    for(int i=0; i<4; i++){
      buzzer.notifyLoudly();                     // buzz notification
      nonBlockingDelay(300);
    }
    display.drawButtonPressMsg();                        // prompts user to press button
    button.delayUntilPressed();                  // wait until button is pressed
    display.drawGoodJobMsg();                            // display good job message (encourages user, positive reinforcement)
    nonBlockingDelay(3000);
    intervalNotification = 3600000;              //return to default notification interval (hourly)
    lastNotification = millis();
  }

  timerEnd = millis();                           //end timing the loop for debugging
  //Serial.println((timerEnd - timerStart));     //print serial port msg for debugging
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Nonblocking delay method: so that we do not stop all processes on the arduino
void nonBlockingDelay(unsigned int delayInterval){
  delayStart = millis();
  while(millis() - delayStart < delayInterval){
    mqttConnect();                /// verify MQTT connection
    mqttClient.loop();            /// check for messages while we wait!
  }
}

// Parser methods - to parse incoming MQTT messages (payloads) based on subscription topic
void parser(char* topic, String message) {
    if (strcmp(topic, TOPIC_SUB_TEMP) == 0) {               // Temperature topic parser
    parseTemperature(message);
  } else if (strcmp(topic, TOPIC_SUB_HUMID) == 0) {         // Humidity topic parser
    parseHumid(message);
  } else if (strcmp(topic, TOPIC_SUB_LIGHT) == 0) {         // Light level topic parser
    parseLight(message);
  } else if (strcmp(topic, TOPIC_SUB_MOOD) == 0) {          // TODO (Sprint 3): Mood topic parser
    parseMood(message);
  } else if (strcmp(topic, TOPIC_SUB_TIMING) == 0) {        // TODO (Sprint 3): Timing topic parser
    parseTiming(message);
  } else if (strcmp(topic, TOPIC_SUB_MOTIVATION) == 0) {    // Motivational messages topic parser
    parseMotivation(message);
  } else if (strcmp(topic, TOPIC_SUB_NOTIFICATION) == 0) {  // Notification messages topic parser
    parseNotification(message);
  }
}

void parseTemperature(String message){
  tempColor = parseColor(message);
}

void parseHumid(String message){
  humidColor = parseColor(message);
}

void parseLight(String message){
  lightColor = parseColor(message);
}

uint16_t parseColor(String message){
  if (message == "TFT_GREEN") {
      return TFT_GREEN;
    } else if (message == "TFT_ORANGE") {
      return TFT_ORANGE;
    } else if (message == "TFT_RED") {
      return TFT_RED;
    }
}

void parseMood(String message){
  //do nothing for now -- feature may be developed in sprint 3
}

void parseTiming(String message){
  //do nothing for now -- feature may be developed in sprint 3
}

void parseMotivation(String message){
  motivationalMessage = message;
  //Serial.println("A new motivational message has arrived!");  //for debugging
}

void parseNotification(String message){
  notificationMessage = message;
  intervalNotification = 0;
  //Serial.println("A new notification has arrived!");  //for debugging  
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MQTT methods - Lookup API documentation here: https://pubsubclient.knolleary.net/api

// MQTT callback method for receiving messages
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];              //append to message string based on payload
  }
  parser(topic, message);                     //call parser method to check topic and parse payload
}

// MQTT connection method
void mqttConnect(){
  if (!mqttClient.connected()) {
    //Serial.println("MQTT connection lost.");   //for debugging
    if (!myWifi.isConnected()) {
      Serial.println("WIFI connection lost."); //for debugging
      myWifi.connect();
    }
    mqttReconnect();
  }
}

// MQTT reconnection method
void mqttReconnect() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT broker..."); //for debugging
    // Attempt to connect to broker
    if (mqttClient.connect(CLIENT_ID, CLIENT_USERNAME, CLIENT_PASSWORD)) {
      // Once connected, publish an announcement to broker...
      Serial.println("Connected to MQTT broker!");    //for debugging
      mqttClient.publish(TOPIC_PUBSUB, "{\"message\": \"Wio Terminal is connected!\"}");
      Serial.println("Published connection message successfully!"); //for debugging
      // Subscribe to topics that App publishes to, to update UI based on user preferences
      mqttSubscribeToAppTopics();
    } else {
      //if not connected:
      Serial.println("... connection to MQTT broker failed! ");
      Serial.print("rc= ");
      Serial.print(mqttClient.state()); // display error message from PubSubClient library
      Serial.println(". Trying again in 1 second");
      nonBlockingDelay(1000); // Wait 1 second before retrying
    }
  }
}

// MQTT method for publishing sensor data
void mqttPublishSensorData() {
  // convert sensor measurements to strings
  dtostrf(temperature, 3, 2, tempStr);
  dtostrf(humidity, 3, 2, humidStr);
  itoa(lightValue, lightStr, 10);  //int base 10
  // publish sensor measurements to broker
  mqttClient.publish(TOPIC_PUB_TEMP, tempStr);    //temperaturePublisher();
  mqttClient.publish(TOPIC_PUB_HUMID, humidStr);  //humidityPublisher();
  mqttClient.publish(TOPIC_PUB_LIGHT, lightStr);  //lightPublisher();
  //Serial.println("Sensor readings published.");                                       // debugging
}

// MQTT method for subscribing to all topics -- called in the connect() / reconnect() methods
void mqttSubscribeToAppTopics() {
  // subscribe to deskBuddy App topics
  mqttClient.subscribe(TOPIC_SUB_TEMP);           //temperatureSubscriber();
  mqttClient.subscribe(TOPIC_SUB_HUMID);          //humiditySubscriber();
  mqttClient.subscribe(TOPIC_SUB_LIGHT);          //lightSubscriber();
  mqttClient.subscribe(TOPIC_SUB_MOOD);           //moodSubscriber();
  mqttClient.subscribe(TOPIC_SUB_TIMING);         //timingSubscriber();
  mqttClient.subscribe(TOPIC_SUB_MOTIVATION);     //motivationSubscriber();
  mqttClient.subscribe(TOPIC_SUB_NOTIFICATION);   //notificationSubscriber();
}