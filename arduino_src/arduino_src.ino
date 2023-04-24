#include "DHT.h" // DHT11 humidity/temperature sensor
#include "TFT_eSPI.h" // Wio terminal screen
#include "button.h" //Reference to the header file for button

//wifi library
#include <rpcWiFi.h>

//MQTT libraries
#include <PubSubClient.h> // MQTT library
#include <WiFiClientSecure.h> // secure Wifi client for PubSub library
#include "networkInfo.h" //secrets file for wifi network (.gitignore)
#include "brokerInfo.h" //secrets file for mqtt broker (.gitignore)
#include "mqttTopics.h" //secrets file for mqtt broker topics (.gitignore)

// PRIVATE MQTT broker settings (such as HiveMQ, Mosquitto) - imported from brokerInfo.h file
const char* mqtt_broker = BROKER_URL; // MQTT broker URL
const int mqtt_port = BROKER_PORT; // MQTT broker port
const char* mqtt_client = CLIENT_ID; // MQTT client ID (unique identifer name)
const char* mqtt_username = CLIENT_USERNAME; // MQTT client username - from MQTT broker
const char* mqtt_password = CLIENT_PASSWORD; // MQTT client password - from MQTT broker
const char* rootCACertificate = ROOT_CA_CERTIFICATE; //SSL certificate - from MQTT broker

// OPTIONAL: PUBLIC MQTT broker settings (WARNING: DO NOT USE THIS FOR SECURE COMMUNICATION!)
#define PUBLICBROKER_CLIENT_ID "myWioDevice"
#define PUBLICBROKER_URL "broker.hivemq.com"
#define PUBLICBROKER_PORT 1883

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

// Create an instance of DHT
DHT dht(A0, DHT11);

//Create instance of button class named "button"
button button;

// initialize TFT LCD
TFT_eSPI screen;

void setup() {
  
  Serial.begin(9600);
  while (!Serial);
  Wire.begin();

  //wifi_setup
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("WiFi connected");

  //mqtt_setup - connects to secure MQTT broker
  wifiSSLClient.setCACert(rootCACertificate); // Set root CA certificate
  mqttClient.setServer(mqtt_broker, mqtt_port);
  mqttClient.setCallback(mqttCallback);

  pinMode(BUTTON_1, INPUT_PULLUP); //Sets the mode of "BUTTON_1" pin to input with an internal pull-up resistor.
  button.begin();

}

void loop() {
  //mqtt_loop - checks for connection to MQTT broker, listens to topic deskBuddy for now
  if (!mqttClient.connected()) {
    mqttReconnect();
  }

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
  
  // Sending data every 30 seconds
  long now = millis();
  if (now - lastMsg > 30000) {
    lastMsg = now;

  lightValue = analogRead(A2);
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  String data="{\"light level\": "+String(lightValue)+","+"\"temperature level\": "+String(temperature)+","+"\"humidity level\": "+String(humidity)+"}";

  if (!mqttClient.publish(mqtt_pub_deskBuddy, data.c_str())) {
    Serial.println("Message failed to send.");
  }
  Serial.printf("Message Send [%s] ", mqtt_pub_deskBuddy);
  Serial.println(data);
  }
  
  mqttClient.loop(); // Receive messages on subscribed topics and trigger mqttCallback function
  delay(10000);
}

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
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
      mqttClient.subscribe(mqtt_pub_deskBuddy);
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