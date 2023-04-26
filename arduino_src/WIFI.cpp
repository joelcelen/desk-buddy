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
  WiFi.begin(ssid, password); // Connecting WiFi
  while (!isConnected()) {
    Serial.println("...not connected, reset device...");
    delay(500);
  }
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
