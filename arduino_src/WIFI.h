/*
*  "WiFi.h" - Custom library for making Arduino Wi-Fi connection.
*  Copyright (c) 2023-2025 Desk Buddy . All rights reserved.
*  - version 1.1 
*
*  Network Requirements: Max Encryption - Open Network with PSK (WPA2-PSK)
*
*  Implementation on Client Side: must include "rpcWiFi.h" in paralel to this library. 
*
*/


#ifndef WIFI_H
#define WIFI_H

class WIFI{
  private:
    const char* ssid;
    const char* password;
    bool connection_status;

  public:

    /* Creates WIFI Object with SSID & PASSWORD ParametersStarts WiFi connection with WEP encryption or WPA2-Personal (WPA-PSK)
     * and overrides default set parameters in "networkinfo.h" file.
     * param-> ssid: Pointer to the SSID string. i.e. "My WiFI SSID"
     * param-> key_password: The key index to set. Valid values are 0-3.
     */
    WIFI(const char* ssid, const char* password);


    /* Initiates WiFi connection with given WEP encryption or WPA2-Personal(WPA-PSK)
     * param-> ssid: Pointer to the SSID string. i.e. "My WiFI SSID"
     * param-> key_password: The key index to set. Valid values are 0-3.
     */
    void connect();

    
    /* Returns true on successful Wi-Fi connection, false otherwise */
    bool isConnected();
    
    
    /* Getters & Setters */
    const char* getSSID() const;
    void setSSID(const char* ssid);
    void setPassword(const char* password);
};

#endif