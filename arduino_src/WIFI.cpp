#include "WIFI.h"
#include <rpcWifi.h>


WIFI::WIFI(){
  connection_status = false;
}

WIFI::WIFI(const char* ssid, const char* password){
  this->ssid = ssid;
  this->password = password;
  connection_status = false;
}

void WIFI::connect(){
  WiFi.begin(ssid, password); // Connecting WiFi
  while (!isConnected()) {
    Serial.println("...not connected, reset device...");
    delay(500);
  }
  connection_status = true;
}

bool WIFI::isConnected(){
  connection_status = (WiFi.status() == WL_CONNECTED);
  return connection_status;
}

const char* WIFI::getSSID() const{
  return ssid;
}

void WIFI::setSSID(const char* ssid){
  this->ssid = ssid;
}

void WIFI::setPassword(const char* password){
  this->password = password;
}