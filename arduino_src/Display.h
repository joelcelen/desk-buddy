#ifndef DISPLAY_H
#define DISPLAY_H

#include <TFT_eSPI.h>

class Display {
  private:
    TFT_eSPI tft;

  public:
    Display();
    void init();
    void drawLaunchScreen();
    void drawConnectingToWifi(const char* ssid);
    void drawConnectedToWifi();
    void drawConnectingToMQTT();
    void drawConnectedToMQTT();
    void drawPreferencesUpdated();
    void drawDashboard(String tempStr, String humidStr, String lightStr, uint16_t tempColor, uint16_t humidColor, uint16_t lightColor);
    void drawStandUpMsg();
    void drawButtonPressMsg();
    void drawNotificationMsg();
    void drawNotificationMsg(String newNotificationMsg);
    void drawGoodJobMsg();
    void drawGoodJobMsg(int countStandUps);
    void drawMotivationalMsg();
    void drawMotivationalMsg(String userDefinedMsg);
    void drawDeskBuddyLogo(const char* deskBuddyLogo);
    void drawAuthorsMsg();
};

#endif