#include "WIFI.h"
#include <rpcWifi.h>

// Constructor with user-defined SSID & Password for Wi-Fi
WIFI::WIFI(const char* ssid, const char* password){
  this->ssid = ssid;
  this->password = password;
  connection_status = false;
}


// Method implementation: Wi-Fi Connection
void WIFI::connect(){
  WiFi.begin(ssid, password);                               // Connecting to Wi-Fi (first attempt)
  Serial.println("Attempting WIFI connection!");            // for debugging
  while (!isConnected()) {
    Serial.println("... connection to WIFI failed, attempting to reconnect -- reset device if this message persists..."); //for debugging
    delay(500);
    WiFi.begin(ssid, password);                             // Reconnect if first attempt was not successful
  }
  Serial.println("Connected to WIFI network!");             // for debugging
  connection_status = true;
}


// Method implementation: to check Wi-Fi Connection status
bool WIFI::isConnected(){
  connection_status = (WiFi.status() == WL_CONNECTED);
  return connection_status;
}


// Getters & Setters implementation
const char* WIFI::getSSID() const{
  return ssid;
}

void WIFI::setSSID(const char* ssid){
  this->ssid = ssid;
}

void WIFI::setPassword(const char* password){
  this->password = password;
}
