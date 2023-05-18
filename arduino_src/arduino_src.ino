/**************************************************************
 * Project: Desk Buddy - Wio Controller v1.0
 * Description: A smart desk assistant built with Arduino and connected to the cloud. It helps you monitor your environment and stay productive.
 * 
 * Gitlab Home Page: https://git.chalmers.se/courses/dit113/2023/group-8/desk-buddy/-/wikis/home
 * Authors: Nasit Vurgun, Rizwan Rafique, Joel Celen, Ahmad Haj Ahmad, Malte Bengtsson, Karl Eriksson
 * Contact: gus***na@student.gu.se - replace *** with the first three letters of the first author's last name
 * Date: May 15, 2023
 * 
 * Copyright (c) 2023 Desk Buddy
 * 
 * License: MIT License
 **************************************************************/

//WiFi libraries
#include "WIFI.h"                              // Wi-Fi Custom library (deskBuddy)
#include <rpcWiFi.h>                           // Wi-Fi System library
#include "NetworkInfo.h"                       // secrets file contains wifi SSID and PASSWORD (.gitignore)

//MQTT libraries. See API info here: https://pubsubclient.knolleary.net/api
#include <PubSubClient.h>                      // MQTT library
#include <WiFiClientSecure.h>                  // secure Wifi client for PubSub library
#include "MqttTopics.h"                        // MQTT broker topics
#include "BrokerInfo.h"                        // secrets file for mqtt broker (.gitignore)

// Sensor libraries
#include "DHT.h"                               // DHT sensor library (temperature and humidity)   
#include "Sensor.h"                            // Sensor class for storing sensor values, preferences, indicator colors
#include "Button.h"                            // Button class

// Actuator libraries
#include "Buzzer.h"                            // Buzzer class
#include "LED.h"                               // LED class

// Display libraries
#include "Display.h"                           // Display class

// Event libraries
#include "Event.h"                             // Events class

/*******************************************************************************************************************/
// pins for sensors and actuators
const int DHT_PIN = A0;                        // DHT: Temperature and Humidity sensor pin
const int LIGHT_PIN = A2;                      // Light sensor pin
const int LED_PIN = D3;                        // Color LED pin
const int BUTTON_PIN = WIO_5S_PRESS;           // Wio 5-switch button pin
const int BUZZER_PIN = WIO_BUZZER;             // Wio buzzer pin

// initializing WIFI conection
WIFI myWifi(SSID, PASSWORD);                   // Create Wi-Fi Instance with SSID & Password

// initializing MQTT connection
WiFiClientSecure wifiSSLClient;                // WIFI client for PubSub with SSL/TLS cryptographic protocols
PubSubClient mqttClient(wifiSSLClient);        // MQTT client with SSL/TLS communication protocols
const int MQTT_BUFFER_SIZE = 256;              // MQTT payload buffer size (default is 256)
char message[MQTT_BUFFER_SIZE];                // MQTT message buffer

// initializing Sensors
DHT dht(DHT_PIN, DHT11);                       // Create an instance of DHT (temperature and humidity) sensor
Button button;                                 // Create an instance of button

// initializing Sensor objects, measurement variables, string conversions
Sensor temperature("25.0", 1.5, 3.0);          // Create Sensor object with preference 25.0 degC, green indicator within 1.5 degC, orange within 1.5-3.0 degC, and red if >3.0 degC
Sensor humidity("35.0", 5.0, 10.0);            // Create Sensor object with preference 35.0 %, green indicator within 5.0 %, orange within 5.0-10.0 %, and red if >10.0 %
Sensor light("900.0", 200.0, 400.0);           // Create Sensor object with preference 900.0 lx, green indicator within 200.0 lx, orange within 200.0-400.0 lx, and red if >400.0 lx
float temperatureValue;                        // stores raw temperature sensor readings
float humidityValue;                           // stores raw humidity sensor readings
int lightValue;                                // stores raw light sensor readings
char tempStr[8], humidStr[8], lightStr[8];     // used in conversion of sensor data to string form

// initializing Actuators
LED colorLED(LED_PIN);                         // Create an intance of color LED
Buzzer buzzer;                                 // Create an instance of buzzer

// Event timer - for loop
unsigned long timerStart;                      // timer for debugging purposes
unsigned long timerEnd;                        // timer for debugging purposes
unsigned long delayStart;                      // timer for custom nonblocking delay method (so MQTT can keep looping)

// GUI update intervals are intentionally short for demo purposes --------------------------------  values for Demo:
const unsigned long INTERVAL_STAND_UP = 3600000;      // time interval to display stand up messages          3600000  (default: 1 hour)
const unsigned long INTERVAL_MOTIVATE = 1800000;      // time interval to display motivational messages      1800000  (default: 30 minutes)
const unsigned long INTERVAL_PUBLISH = 1000;          // time interval to publish sensor data                1000     (default: 1 second)
const unsigned long INTERVAL_DISPLAY = 1000;          // time interval to refresh dashboard display          1000     (default: 1 second)
const unsigned long INTERVAL_NOTIFICATION = 21600000; // time interval to send notification to user          21600000 (default: 6 hours)
const unsigned long INTERVAL_ONE_TIME = 777;          // time interval for one-time events
/* ADD YOUR OWN EVENT INTERVALS HERE */

// Events (deskBuddy core features)
Event standUp(INTERVAL_STAND_UP);               // define stand up event/VIEW
Event notification(INTERVAL_NOTIFICATION);      // define notification event/VIEW
Event motivate(INTERVAL_MOTIVATE);              // define motivation event/VIEW
Event publish(INTERVAL_PUBLISH);                // define publish event/VIEW
Event refreshDisplay(INTERVAL_DISPLAY);         // define refresh GUI dashboard event/VIEW
/* ADD YOUR OWN EVENTS HERE */

// initialize TFT LCD display
Display tft;
int count = 0;

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
  pinMode(BUTTON_PIN, INPUT_PULLUP);              // setup button sensor (wio 5-switch)
  pinMode(BUZZER_PIN, OUTPUT);                    // setup buzzer actuator
  pinMode(LED_PIN, OUTPUT);                       // setup LED

  tft.drawDeskBuddyLogo(deskBuddyLogo);           // display desk buddy logo
  nonBlockingDelay(1500);
  tft.drawAuthorsMsg();                           // display list of authors of deskBuddy project
  nonBlockingDelay(2000);
}

void loop() {
  timerStart = millis();                          // start timing the loop for debugging purposes -- takes about 500 ms to get through a loop right now!

  //Connect MQTT - Verify connection
  mqttConnect();                                  // verify MQTT connection
  mqttClient.loop();                              // Retrieve messages on subscribed topics and trigger mqttCallback function

  // read sensors data
  temperatureValue = dht.readTemperature();
  humidityValue = dht.readHumidity();
  lightValue = analogRead(LIGHT_PIN);

  // convert readings to strings
  dtostrf(temperatureValue, 3, 2, tempStr);
  dtostrf(humidityValue, 3, 2, humidStr);
  itoa(lightValue, lightStr, 10);

  // update Sensor objects
  temperature.setValue(tempStr);
  humidity.setValue(humidStr);
  light.setValue(lightStr);

  // Controller: Updates Views based on Model (uses non-blocking timers and mqtt event listeners)
  if (standUp.shouldExecute()) {
    standUpEventSequence();                       // VIEW: StandUp
  }

  if (motivate.shouldExecute()) {
    motivationEventSequence();                    // VIEW: Motivate
  }

  if (notification.shouldExecute()) {
    notificationEventSequence();                  // VIEW: Notification
  }

  if (publish.shouldExecute()) {
    publishSensorsEventSequence();                // VIEW: Publish
  }

  if (refreshDisplay.shouldExecute()) {
    updateDashboardEventSequence();               // VIEW: UpdateDashboard
  }

  timerEnd = millis();                            // end timing the loop for debugging
  //Serial.println((timerEnd - timerStart));      // print serial port msg for debugging -- loop takes around 500 ms
}

/*******************************************************************************************************************/
// EVENT METHODS: We are using the main as a controller to check for triggers to transition between views

// VIEW: StandUp (trigger: default/ user defined timing interval) 
// prompts user to stand up, internal transitions do not proceed until user presses button to confirm
void standUpEventSequence(){
  tft.drawStandUpMsg();                           // display standup messsage, user will be prompted to click button
  buzzer.notifyLoudly();                          // buzz notification
  nonBlockingDelay(1000);
  tft.drawButtonPressMsg();                       // prompts user to press button
  colorLED.turnOn();                              // turn on color LED
  while(!button.checkState()){                    // wait until button is pressed
      nonBlockingDelay(100);
  }
  standUp.setLastEvent(millis());                 // reset timer (do this before drawing good job msg to increment counter by 1)
  tft.drawGoodJobMsg(standUp.getCount());         // display good job message (encourages user, positive reinforcement)
  nonBlockingDelay(3000);
  colorLED.turnOff();                             // turn off color LED
  // turn off one-time standUp msg
  if(standUp.getInterval() == INTERVAL_ONE_TIME){
    standUp.setInterval(INTERVAL_STAND_UP);
  }
  tft.clearScreen();
}

// VIEW: Motivate (trigger: default/ user defined timing interval) 
// displays user defined motivational message (or default if null/empty string)
void motivationEventSequence(){
  tft.drawMotivationMsg(motivate.getMessage());   // display motivational message
  nonBlockingDelay(2500);
  motivate.setLastEvent(millis());                // reset timer
  // turn off one-time motivation msg
  if(motivate.getInterval() == INTERVAL_ONE_TIME){
    motivate.setInterval(INTERVAL_MOTIVATE);
  }
  tft.clearScreen();
}

// VIEW: Publish (trigger: default/ user defined timing interval) 
// Publishes readings from environmental sensors to MQTT broker
void publishSensorsEventSequence(){
  mqttPublishSensorData();                        // publish sensor data
  publish.setLastEvent(millis());                 // reset timer
}

// VIEW: UpdateDashboard (trigger: default/ user defined timing interval)
// Updates dashboard on device with environmental sensor readings and indicators that change color based on user preferences
void updateDashboardEventSequence(){
  tft.clearScreenOnce();
  //update dashboard of sensor readings and indicators based on user preferences
  tft.drawDashboard(
    temperature.getValue(),                       // displays live temperature sensor reading
    humidity.getValue(),                          // displays live humidity sensor reading
    light.getValue(),                             // displays live light sensor reading
    temperature.getIndicatorColor(),              // updates temperature indicator based on user defined target sent by MQTT from App
    humidity.getIndicatorColor(),                 // updates humidity indicator based on user defined target sent by MQTT from App
    light.getIndicatorColor());                   // updates light indicator based on user defined target sent by MQTT from App
    refreshDisplay.setLastEvent(millis());        // reset timer
}

// VIEW: Notification (trigger: default/ user defined timing interval)
// Notifies users with user defined notification - internal transitions are on hold until user presses button to confirm
void notificationEventSequence(){
  tft.drawNotificationMsg(notification.getMessage());  // display notification message
  for(int i=0; i<4; i++){
    buzzer.notifyLoudly();                             // buzz notification
    nonBlockingDelay(300);
  }
  tft.drawButtonPressMsg();                            // prompts user to press button
  while(!button.checkState()){                         // wait until button is pressed
      nonBlockingDelay(100);
  }
  tft.drawGoodJobMsg();                                // display good job message (encourages user, positive reinforcement)
  nonBlockingDelay(3000);
  notification.setLastEvent(millis());
  // turn off one-time notification msg
  if(notification.getInterval() == INTERVAL_ONE_TIME){
    notification.setInterval(INTERVAL_NOTIFICATION);
  }
  tft.clearScreen();
}

// VIEW: <New feature 1>
// Create your own void method below which describes the event sequence (such as to play music, or change timing intervals)

// VIEW: <New feature 2>
// Create your own void method below which describes the event sequence (such as to play music, or change timing intervals)

/*******************************************************************************************************************/
// ***Nonblocking delay method***: a crucial method for creating delays that do not stop all processes on the arduino

void nonBlockingDelay(unsigned int delayInterval){
  delayStart = millis();
  while(millis() - delayStart < delayInterval){
    mqttConnect();                                          // verify MQTT connection, and more importantly
    mqttClient.loop();                                      // check for messages while we wait!!!
  }
}

/*******************************************************************************************************************/
// Parser methods - to parse incoming MQTT messages (payloads) based on subscription topic

void parser(const char* topic, const char* message) {
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
  } else if (strcmp(topic, TOPIC_SUB_STANDUP) == 0) {       // StandUp messages topic parser
    parseStandUp(message);
  } else if (strcmp(topic, TOPIC_SUB_TIMING) == 0) {        // Timing intervals topic parser
    parseTiming(message);
  }
  // PARSER: <New feature 1>
  // Call your own parser here in the if else block
  
  // PARSER: <New feature 2>
  // Call your own parser here in the if else block
}

void parseTemperature(const char* message){
  temperature.setPrefValue(message);                       // Set temperature preference
}

void parseHumid(const char* message){
  humidity.setPrefValue(message);                          // Set humidity preference
}

void parseLight(const char* message){
  light.setPrefValue(message);                             // Set light preference
}

void parseMotivation(const char* message){
  motivate.setMessage(message);                            // sets user defined motivational message, empty string "" restores default
  motivate.setInterval(INTERVAL_ONE_TIME);                 // one-time motivation message (interval set back to default in motivationEventSequence)
  //Serial.println("A new motivation has arrived!");       // for debugging
}

void parseNotification(const char* message){
  notification.setMessage(message);                        // sets user defined notification message, empty string "" restores default
  notification.setInterval(INTERVAL_ONE_TIME);             // one-time notification message (interval set back to default in notificationEventSequence)
  //Serial.println("A new notification has arrived!");     // for debugging  
}

void parseStandUp(const char* message){
  standUp.setInterval(INTERVAL_ONE_TIME);                  // one-time standUp message (interval set back to default in standUpEventSequence)
  //Serial.println("A new standup message has arrived!");  // for debugging  
}

void parseTiming(const char* message){
  char messageType = message[0];
  
  Serial.println("A new timing message has arrived!");     // for debugging 
  Serial.println("Before: ");
  Serial.print("intervalStandUp= ");
  Serial.println(standUp.getInterval()/60000);
  Serial.print("intervalMotivate= ");
  Serial.println(motivate.getInterval()/60000);
  Serial.print("intervalNotification= ");
  Serial.println(notification.getInterval()/60000);
  Serial.print("intervalPublish= ");
  Serial.println(publish.getInterval()/60000);
  Serial.print("intervalDisplay= ");
  Serial.println(refreshDisplay.getInterval()/60000);
  

  switch (messageType) {
    case '0':
      // restore default values
      standUp.setInterval(INTERVAL_STAND_UP);
      motivate.setInterval(INTERVAL_MOTIVATE);
      notification.setInterval(INTERVAL_NOTIFICATION);
      break;

    case '1':
      // modify intervalStandUp
      standUp.setInterval(atol(message + 1));              // parse to long and set standUp interval
      break;

    case '2':
      // modify intervalMotivate
      motivate.setInterval(atol(message + 1));             // parse to long and set standUp interval
      break;

    case '3':
      // modify intervalNotification
      notification.setInterval(atol(message + 1));         // parse to long and set standUp interval
      break;

    case '4':
      // turn off standUp events
      standUp.setInterval(ULONG_MAX);                      // set standUp interval to largest unsigned long (4294967295) = 49.710269 days
      break;

    case '5':
      // turn off motivational messages
      motivate.setInterval(ULONG_MAX);                     // set motivate interval to largest unsigned long (4294967295) = 49.710269 days
      break;

    case '6':
      // turn off notifications
      notification.setInterval(ULONG_MAX);                 // set notification interval to largest unsigned long (4294967295) = 49.710269 days
      break;

    case '7':
      // turn off all events by setting everything to the largest unsigned long
      standUp.setInterval(ULONG_MAX);                      // set standUp interval to largest unsigned long (4294967295) = 49.710269 days
      motivate.setInterval(ULONG_MAX);                     // set motivate interval to largest unsigned long (4294967295) = 49.710269 days
      notification.setInterval(ULONG_MAX);                 // set notification interval to largest unsigned long (4294967295) = 49.710269 days
      break;

    case '8':
      // modify intervalPublish
      publish.setInterval(atol(message + 1));              // parse to long and set publish interval
      break;
    
    case '9':
      // modify intervalDisplay
      refreshDisplay.setInterval(atol(message + 1));       // parse to long and set display interval
      break;

    default:
      // handle unexpected messages
      Serial.print("Received unexpected message: ");
      Serial.println(message);
      break;
  }
  
  Serial.println("After: ");                               // for debugging 
  Serial.print("intervalStandUp= ");
  Serial.println(standUp.getInterval()/60000);
  Serial.print("intervalMotivate= ");
  Serial.println(motivate.getInterval()/60000);
  Serial.print("intervalNotification= ");
  Serial.println(notification.getInterval()/60000);
  Serial.print("intervalPublish= ");
  Serial.println(publish.getInterval()/60000);
  Serial.print("intervalDisplay= ");
  Serial.println(refreshDisplay.getInterval()/60000);
}

// PARSER: <New feature 1>
// Create your own parser method here

// PARSER: <New feature 2>
// Create your own parser method here

/*******************************************************************************************************************/
// MQTT methods - Lookup API documentation here: https://pubsubclient.knolleary.net/api

// MQTT callback method for receiving messages - this needs to be short and sweet to avoid processing slowdowns
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  // Clear the message buffer
  memset(message, 0, sizeof(message));

  // Copy the payload to the message buffer
  memcpy(message, payload, min(length, MQTT_BUFFER_SIZE - 1));
  message[min(length, MQTT_BUFFER_SIZE - 1)] = '\0';

  // Create immutable objects for the topic and message
  const String topicStr(topic);
  const String payloadStr(message);
  
  // Call the parser function to check topic and payload message
  parser(topicStr.c_str(), payloadStr.c_str());
}

// MQTT connection method: check MQTT connection. If lost, check WIFI connection & restore WIFI connection. Then restore MQTT connection.
void mqttConnect(){
  if (!mqttClient.connected()) {                           // check MQTT connection first
    //Serial.println("MQTT connection lost.");             // for debugging
    if (!myWifi.isConnected()) {                           // check WIFI connection
      Serial.println("WIFI connection lost.");             // for debugging
      myWifi.connect();                                    // restore WIFI connection
    }
    mqttReconnect();                                       // restore MQTT connection
  }
}

// MQTT reconnection method
void mqttReconnect() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT broker...");                          // for debugging
    // Attempt to connect to broker
    if (mqttClient.connect(CLIENT_ID, CLIENT_USERNAME, CLIENT_PASSWORD)) {   // connect to broker with client ID (unique for each connection instance), username and password
      // Once connected, publish an announcement to broker...
      Serial.println("Connected to MQTT broker!");                           // for debugging

      //publish a message from the Wio Terminal
      mqttClient.publish(TOPIC_PUBSUB, "{\"message\": \"Wio Terminal is connected!\"}");
      Serial.println("Published connection message successfully!");          // for debugging

      // Subscribe to all topics that the deskBuddy Android App publishes to, defined in MqttTopics.h
      mqttSubscribeToAppTopics();

    } else {
      //if not connected:
      Serial.println("... connection to MQTT broker failed! ");
      Serial.print("rc= ");
      Serial.print(mqttClient.state());                                      // display error message from PubSubClient library
      Serial.println(". Trying again in 1 second");
      delay(1000);                                                           // Wait 1 second before retrying
    }
  }
}

// MQTT method for publishing sensor data to topics defined in MqttTopics.h
void mqttPublishSensorData() {
  mqttClient.publish(TOPIC_PUB_TEMP, temperature.getValue());                // temperaturePublisher();
  mqttClient.publish(TOPIC_PUB_HUMID, humidity.getValue());                  // humidityPublisher();
  mqttClient.publish(TOPIC_PUB_LIGHT, light.getValue());                     // lightPublisher();
  //Serial.println("Sensor readings published.");                            // debugging

  // PUBLISHER: <New feature 1>
  // PUBLISH to your own topic here

  // PUBLISHER: <New feature 2>
  // PUBLISH to your own topic here
}

// MQTT method for subscribing to all topics defined in mqttTopics.h-- called by the connect() / reconnect() methods
void mqttSubscribeToAppTopics() {
  mqttClient.subscribe(TOPIC_SUB_TEMP);                                      // temperatureSubscriber();
  mqttClient.subscribe(TOPIC_SUB_HUMID);                                     // humiditySubscriber();
  mqttClient.subscribe(TOPIC_SUB_LIGHT);                                     // lightSubscriber();
  mqttClient.subscribe(TOPIC_SUB_MOTIVATION);                                // motivationSubscriber();
  mqttClient.subscribe(TOPIC_SUB_NOTIFICATION);                              // notificationSubscriber();
  mqttClient.subscribe(TOPIC_SUB_STANDUP);                                   // standUpSubscriber();
  mqttClient.subscribe(TOPIC_SUB_TIMING);                                    // timingSubscriber();

  // SUBSCRIBER: <New feature 1>
  // Subscribe to your own topic here

  // SUBSCRIBER: <New feature 2>
  // Subscribe to your own topic here
}