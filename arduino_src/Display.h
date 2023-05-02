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
    void drawNotificationMsg(String notificationMsg);
    void drawNotificationMsgDefault();
    void drawNotificationMsgUserDefined(String notificationMsg);
    void drawGoodJobMsg();
    void drawGoodJobMsg(int countStandUps);
    void drawMotivationMsg(String motivationMsg);
    void drawMotivationMsgDefault();
    void drawMotivationMsgUserDefined(String motivationMsg);
    void drawDeskBuddyLogo(const char* deskBuddyLogo);
    void drawAuthorsMsg();
};

#endif