/**************************************************************
* "Display.h" - deskBuddy application specific library containing 
*  drawMethods for deskBuddy GUI on the Wio Seeed Terminal device.
 **************************************************************/
#ifndef DISPLAY_H
#define DISPLAY_H

#include <TFT_eSPI.h>

class Display {
  private:
    TFT_eSPI tft;
    String lastTempStr;
    String lastHumidStr;
    String lastLightStr;
    int refreshCounter;

  public:
    Display();
    void init();
    void clearScreen();
    void clearScreenOnce();
    void drawLaunchScreen();
    void drawConnectingToWifi(const char* ssid);
    void drawConnectedToWifi();
    void drawConnectingToMQTT();
    void drawConnectedToMQTT();
    void drawPreferencesUpdated();
    void drawDashboard(const char* tempStr, const char* humidStr, const char* lightStr, uint16_t tempColor, uint16_t humidColor, uint16_t lightColor);
    void drawStandUpMsg();
    void drawButtonPressMsg();
    void drawNotificationMsg(const char* notificationMsg);
    void drawGoodJobMsg();
    void drawGoodJobMsg(int countStandUps);
    void drawMotivationMsg(const char* motivationMsg);
    void drawDeskBuddyLogo(const char* deskBuddyLogo);
    void drawAuthorsMsg();
};

#endif