/**************************************************************
 * Project: Desk Buddy - Wio Controller v1.0 (Sprint 2 Demo)
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
#include "WIFI.h"                             //Wi-Fi Custom library (deskBuddy)
#include <rpcWiFi.h>                          //Wi-Fi System library
#include "NetworkInfo.h"                      //contains wifi SSID and PASSWORD

//MQTT libraries. See API info here: https://pubsubclient.knolleary.net/api
#include <PubSubClient.h>                     // MQTT library
#include <WiFiClientSecure.h>                 // secure Wifi client for PubSub library
#include "BrokerInfo.h"                       //secrets file for mqtt broker (.gitignore)
#include "MqttTopics.h"                       //secrets file for mqtt broker topics (.gitignore)

// Sensor libraries (deskBuddy)
#include "DHT.h"                              //DHT sensor library (temperature and humidity)   
#include "Sensor.h"                           //Sensor class (deskBuddy)
#include "Button.h"                           //Button class (deskBuddy)

// Actuator libraries(deskBuddy)
#include "Buzzer.h"                           //Buzzer class

// Display libraries (deskBuddy)
#include "Display.h"                          //Display class

// Event libraries (deskBuddy)
#include "Event.h"                            //Events class

// initializing WIFI conection
WIFI myWifi(SSID, PASSWORD);                  //Create Wi-Fi Instance with SSID & Password
// initializing MQTT connection
WiFiClientSecure wifiSSLClient;               //wifi client for PubSub with SSL/TLS cryptographic protocols
PubSubClient mqttClient(wifiSSLClient);       //MQTT client with SSL/TLS communication protocols
String message = "";                          //MQTT default message (empty string)

// initializing sensor measurement variables and their string conversions
DHT dht(A0, DHT11);                           //Create an instance of DHT (temperature and humidity) sensor

// initializing Sensors
float temperatureValue;                       //stores temperature sensor readings
float humidityValue;                          //stores humidity sensor readings
int lightValue;                               //stores light sensor readings
char tempStr[8], humidStr[8], lightStr[8];    //used in conversion of sensor data to string form
Sensor temperature(TFT_GREEN);                //holds published temperature readings and dashboard indicator color
Sensor humidity(TFT_ORANGE);                  //holds published temperature readings and dashboard indicator color
Sensor light(TFT_RED);                        //holds published light level readings and dashboard indicator color

// initializing Actuators
Button button;                               //Create an instance of button
Buzzer buzzer;                               //Create an instance of buzzer

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

// Events (deskBuddy core features)
Event standUp(intervalStandUp);              // define stand up event/view
Event notification(intervalNotification);    // define notification event/view
Event motivate(intervalMotivate);            // define motivation event/view
Event publish(intervalPublish);              // define publish event/view
Event refreshDisplay(intervalDisplay);       // define refresh GUI dashboard event/view

// initialize TFT LCD display
Display tft;

// ascii art deskBuddy logo (font: big, layouts: default), from https://www.coolgenerator.com/ascii-text-generator
const char* deskBuddyLogo = "      _           _    ____            _     _       \n"
                       "     | |         | |  |  _ \\          | |   | |      \n"
                       "   __| | ___  ___| | _| |_) |_   _  __| | __| |_   _ \n"
                       "  / _` |/ _ \\/ __| |/ /  _ <| | | |/ _` |/ _` | | | |\n"
                       " | (_| |  __/\\__ \\   <| |_) | |_| | (_| | (_| | |_| |\n"
                       "  \\__,_|\\___||___/_|\\_\\____/ \\__,_|\\__,_|\\__,_|\\__, |\n"
                       "                                                __/ |\n"
                       "                                               |___/ ";

/*******************************************************************************************************************/

void setup() {
  tft.init();                                     // set up Wio Display
  tft.drawLaunchScreen();                         // display GUI when you power on device

  //setup Serial communication
  Serial.begin(115200);                           // open serial port with max bit rate
  //while (!Serial);                              // wait for serial port to open -- useful if debugging things in setup

  tft.drawConnectingToWifi(myWifi.getSSID());     // connecting msg
  myWifi.connect();                               // setup Wi-Fi connection -- based on networkInfo.h parameters
  tft.drawConnectedToWifi();                      // connected msg
  delay(100);                                     // this blocking delay() is the only one in this program! it is purely for visual aesthetics on startup screen

  //setup MQTT connection, defined in BrokerInfo.h
  wifiSSLClient.setCACert(ROOT_CA_CERTIFICATE);   // Set root CA certificate for SSL/TLS encryption
  mqttClient.setServer(BROKER_URL, BROKER_PORT);  // Set MQTT broker URL and port
  mqttClient.setCallback(mqttCallback);           // Set callback method -- what to do when a message is received
  tft.drawConnectingToMQTT();
  mqttConnect();                                  // connect to MQTT (also verify wifi connection)
  nonBlockingDelay(100);
  tft.drawConnectedToMQTT();                      // connected message
  nonBlockingDelay(100);
  tft.drawPreferencesUpdated();                   // we will update user preferences once the loop starts, but display it now

  //setup sensors and actuators
  dht.begin();
  pinMode(WIO_5S_PRESS, INPUT_PULLUP);            // setup button sensor (wio 5-switch)
  pinMode(WIO_BUZZER, OUTPUT);                    // setup buzzer actuator

  tft.drawDeskBuddyLogo(deskBuddyLogo);           // display desk buddy logo
  nonBlockingDelay(1500);
  tft.drawAuthorsMsg();                           // display list of authors of deskBuddy project
  nonBlockingDelay(2000);
}

void loop() {
  timerStart = millis();                          //start timing the loop for debugging purposes -- takes about 500-600ms to get through a loop right now!

  //Connect MQTT - Verify connection
  mqttConnect();                                  // verify MQTT connection
  mqttClient.loop();                              // Retrieve messages on subscribed topics and trigger mqttCallback function

  // read sensors data
  temperatureValue = dht.readTemperature();
  humidityValue = dht.readHumidity();
  lightValue = analogRead(A2);

  // convert readings to strings
  dtostrf(temperatureValue, 3, 2, tempStr);
  dtostrf(humidityValue, 3, 2, humidStr);
  itoa(lightValue, lightStr, 10);

  // update Sensor objects
  temperature.setValue(tempStr);
  humidity.setValue(humidStr);
  light.setValue(lightStr);

  // Controller: Updates View based on Model (uses non-blocking timers and mqtt event listeners)
  if (standUp.shouldExecute()) {
    standUpEventSequence();                       //STATE: StandUp
    
  } else if (motivate.shouldExecute()) {
    motivationEventSequence();                    //STATE: Motivate

  } else if (publish.shouldExecute()) {
    publishSensorsEventSequence();                //STATE: Publish

  } else if (refreshDisplay.shouldExecute()) {
    updateDashboardEventSequence();               //STATE: UpdateDashboard

  } else if (notification.shouldExecute()) {
    notificationEventSequence();                  //STATE: Notify
  }

  timerEnd = millis();                            //end timing the loop for debugging
  //Serial.println((timerEnd - timerStart));        //print serial port msg for debugging -- loop takes around 500 ms
}

/*******************************************************************************************************************/
// EVENT METHODS: We are using the main as a controller to check for triggers to transition between states

// STATE: StandUp (trigger: default/ user defined timing interval) 
// prompts user to stand up, internal transitions do not proceed until user presses button to confirm
void standUpEventSequence(){
  tft.drawStandUpMsg();                           // display standup messsage, user will be prompted to click button
  buzzer.notifyLoudly();                          // buzz notification
  nonBlockingDelay(1000);
  tft.drawButtonPressMsg();                       // prompts user to press button
  while(!button.checkState()){                    // wait until button is pressed
      nonBlockingDelay(100);
  }
  standUp.setLastEvent(millis());                 //reset timer (before the event to increment counter by 1)
  tft.drawGoodJobMsg(standUp.getCount());         // display good job message (encourages user, positive reinforcement)
  nonBlockingDelay(2000);
}

// STATE: Motivate (trigger: default/ user defined timing interval) 
// displays user defined motivational message (or default if null/empty string)
void motivationEventSequence(){
    tft.drawMotivationMsg((motivate.getMessage())); //user-defined motivational message
    nonBlockingDelay(2500);
    motivate.setLastEvent(millis());              //reset timer
}

// STATE: Publish (trigger: default/ user defined timing interval) 
// Publishes readings from environmental sensors to MQTT broker
void publishSensorsEventSequence(){
  mqttPublishSensorData();                        //publish sensor data
  publish.setLastEvent(millis());                 //reset timer
}

// STATE: UpdateDashboard (trigger: default/ user defined timing interval)
// Updates dashboard on device with environmental sensor readings and indicators that change color based on user preferences
void updateDashboardEventSequence(){
  //update dashboard of sensor readings and indicators based on user preferences
  tft.drawDashboard(
    temperature.getValue(),                       //displays live temperature sensor reading
    humidity.getValue(),                          //displays live humidity sensor reading
    light.getValue(),                             //displays live light sensor reading
    temperature.getIndicatorColor(),              //updates temperature indicator based on user defined target sent by MQTT from App
    humidity.getIndicatorColor(),                 //updates humidity indicator based on user defined target sent by MQTT from App
    light.getIndicatorColor());                   //updates light indicator based on user defined target sent by MQTT from App
  refreshDisplay.setLastEvent(millis());          //reset timer
}

// STATE: Notify (trigger: default/ user defined timing interval)
// Notifies users with user defined notification - internal transitions are on hold until user presses button to confirm
void notificationEventSequence(){
  tft.drawNotificationMsg(notification.getMessage());  //display notification message
    for(int i=0; i<4; i++){
      buzzer.notifyLoudly();                           // buzz notification
      nonBlockingDelay(300);
    }
    tft.drawButtonPressMsg();                          // prompts user to press button
    button.delayUntilPressed();                        // wait until button is pressed
    nonBlockingDelay(300);
    tft.drawGoodJobMsg();                              // display good job message (encourages user, positive reinforcement)
    nonBlockingDelay(3000);
    notification.setInterval(3600000);                 //return to default notification interval (hourly)
    notification.setLastEvent(millis());
}

/*******************************************************************************************************************/
// ***Nonblocking delay method***: a crucial method for creating delays that do not stop all processes on the arduino*

void nonBlockingDelay(unsigned int delayInterval){
  delayStart = millis();
  while(millis() - delayStart < delayInterval){
    mqttConnect();                // verify MQTT connection, and more importantly
    mqttClient.loop();            // check for messages while we wait!!! (SUPER IMPORTANT)
  }
}

/*******************************************************************************************************************/
// Parser methods - to parse incoming MQTT messages (payloads) based on subscription topic

void parser(char* topic, String message) {
    if (strcmp(topic, TOPIC_SUB_TEMP) == 0) {               // Temperature topic parser
    parseTemperature(message);
  } else if (strcmp(topic, TOPIC_SUB_HUMID) == 0) {         // Humidity topic parser
    parseHumid(message);
  } else if (strcmp(topic, TOPIC_SUB_LIGHT) == 0) {         // Light level topic parser
    parseLight(message);
  } else if (strcmp(topic, TOPIC_SUB_MOTIVATION) == 0) {    // Motivational messages topic parser
    parseMotivation(message);
  } else if (strcmp(topic, TOPIC_SUB_NOTIFICATION) == 0) {  // Notification messages topic parser
    parseNotification(message);
  } else if (strcmp(topic, TOPIC_SUB_MOOD) == 0) {          // TODO: Mood topic parser (Sprint 3)
    parseMood(message);
  } else if (strcmp(topic, TOPIC_SUB_TIMING) == 0) {        // TODO: Timing topic parser (Sprint 3)
    parseTiming(message);
  }
}

void parseTemperature(String message){
  temperature.setIndicatorColor(parseColor(message));
}

void parseHumid(String message){
  humidity.setIndicatorColor(parseColor(message));
}

void parseLight(String message){
  light.setIndicatorColor(parseColor(message));
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

void parseMotivation(String message){
  motivate.setMessage(message.c_str());
  //Serial.println("A new motivation has arrived!");    //for debugging
}

void parseNotification(String message){
  notification.setMessage(message.c_str());
  notification.setInterval(0);                          // notification should display almost immediately
  //Serial.println("A new notification has arrived!");  //for debugging  
}

void parseMood(String message){
  //TODO: feature may be developed in sprint 3
}

void parseTiming(String message){
  //TODO: feature may be developed in sprint 3
}

/*******************************************************************************************************************/
// MQTT methods - Lookup API documentation here: https://pubsubclient.knolleary.net/api

// MQTT callback method for receiving messages - this needs to be short and sweet to avoid processing slowdowns
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
    //Serial.println("MQTT connection lost."); //for debugging
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

      //publish a message from the Wio Terminal
      mqttClient.publish(TOPIC_PUBSUB, "{\"message\": \"Wio Terminal is connected!\"}");

      Serial.println("Published connection message successfully!"); //for debugging

      // Subscribe to all topics that the deskBuddy Android App publishes to, defined in mqttTopics.h
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
  mqttClient.publish(TOPIC_PUB_TEMP, temperature.getValue());    //temperaturePublisher();
  mqttClient.publish(TOPIC_PUB_HUMID, humidity.getValue());  //humidityPublisher();
  mqttClient.publish(TOPIC_PUB_LIGHT, light.getValue());  //lightPublisher();
  //Serial.println("Sensor readings published.");                                       // debugging
}

// MQTT method for subscribing to all topics defined in mqttTopics.h-- called by the connect() / reconnect() methods
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