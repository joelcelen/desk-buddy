#include "Display.h"

Display::Display(){};

//initialize TFT LCD display
void Display::init(){
  tft.begin();
}

void Display::drawLaunchScreen(){
  tft.setRotation(3);  // Set the display rotation to 270 degrees
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setTextFont(1);
  tft.println("");
  tft.setCursor(20, 10);
  tft.println("Launching deskBuddy...");
}

void Display::drawConnectingToWifi(const char* ssid){
  tft.setCursor(20, 20);
  tft.print("Connecting to Wi-Fi network");
  if (strlen(ssid) > 5) {  // screen doesn't like printing Wi-Fi names more than 5 characters
    tft.print("... ");
  } else {
    tft.print("(");
    tft.print(ssid);       // only print if have a short wifi name (otherwise goes off the screen)
    tft.print(")... ");
  }
}

void Display::drawConnectedToWifi(){
  tft.println("connected!");
}

void Display::drawConnectingToMQTT(){
  tft.setCursor(20, 30);
  tft.print("Connecting to MQTT broker... ");
}

void Display::drawConnectedToMQTT(){
  tft.println("connected!");
}

void Display::drawPreferencesUpdated(){
  tft.setCursor(20, 40);
  tft.println("Preferences updated!");           
}

void Display::drawDashboard(String tempStr, String humidStr, String lightStr, uint16_t tempColor, uint16_t humidColor, uint16_t lightColor) {
  //clear screen and set text properties
  tft.fillScreen(TFT_BLACK);                  //clear screen
  tft.setTextSize(2);                         //set text size
  tft.setTextColor(TFT_WHITE);                //set text color
  
  //draw temperature box and text
  tft.fillRect(10, 15, 20, 20, tempColor);
  tft.setTextFont(2);
  tft.drawString("Temperature", 40, 10);
  tft.setTextFont(4);
  tft.drawString(tempStr + " C", 40, 40);

  //draw humidity box and text
  tft.fillRect(10, 90, 20, 20, humidColor);
  tft.setTextFont(2);
  tft.drawString("Humidity", 40, 85);
  tft.setTextFont(4);
  tft.drawString(humidStr + " %", 40, 115);

  //draw light level box and text
  tft.fillRect(10, 165, 20, 20, lightColor);
  tft.setTextFont(2);
  tft.drawString("Light Level", 40, 160);
  tft.setTextFont(4);
  tft.drawString(lightStr + " lx", 40, 190);
}

void Display::drawStandUpMsg() {
  //clear screen and set text properties
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  //draw message for standing up and stretching
  tft.setTextFont(1);
  tft.setCursor(30, 50);
  tft.println("You have been sitting");
  tft.setCursor(30, 110);
  tft.println(" for too long!! o_o");
  tft.setCursor(30, 170);
  tft.println("Stand up and stretch!");
}

void Display::drawNotificationMsg(String notificationMsg) {
    if (notificationMsg.length() == 0) {         //draw default notification msg
      drawNotificationMsgDefault();              
    }else{                                                 //user-defined motivational message
      drawNotificationMsgUserDefined(notificationMsg);
    }
  
}

void Display::drawNotificationMsgDefault() {
  //clear screen and set text properties
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  //draw message for standing up and stretching
  tft.setTextFont(1);
  tft.setCursor(30, 50);
  tft.println("ATTENTION!!!");
  tft.setCursor(30, 110);
  tft.println("Your laundry is ready!");
  tft.setCursor(30, 170);
  tft.println("Come and get it.");
}

void Display::drawNotificationMsgUserDefined(String notificationMsg){
  //clear screen and set text properties
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  //draw message for standing up and stretching
  tft.setTextFont(1);
  tft.setCursor(30, 50);
  tft.println("ATTENTION!!!");
  if(notificationMsg.length() < 25){
    tft.setCursor(30, 130);
    tft.println(notificationMsg);
  }else if(notificationMsg.length()>25 && notificationMsg.length()<50){
    tft.setCursor(30, 110);
    tft.println(notificationMsg.substring(0, 24));
    tft.setCursor(30, 170);
    tft.println(notificationMsg.substring(25));
  }else{
    tft.setCursor(30, 110);
    tft.println(notificationMsg.substring(0, 24));
    tft.setCursor(30, 170);
    tft.println(notificationMsg.substring(25, 49));
    tft.setCursor(30, 170);
    tft.println(notificationMsg.substring(50));
  }
}

void Display::drawButtonPressMsg(){
  //draw message instructing button press
  tft.setTextSize(1);
  tft.setCursor(290, 220);
  tft.println("press");
  tft.setCursor(294, 230);
  tft.println("(OK)");
}

void Display::drawGoodJobMsg() {
  //draw good job message after user responds to notification
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(3);
  tft.setTextColor(TFT_WHITE);
  tft.setTextFont(1);
  tft.setCursor(30, 80);
  tft.println("Good Job!!!");
  tft.setCursor(100, 180);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.println("Done. Now, let's get back to work!");
  tft.setTextColor(TFT_WHITE);
}

void Display::drawGoodJobMsg(int countStandUps) {
  //draw good job message after user responds to stand up message
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(3);
  tft.setTextColor(TFT_WHITE);
  tft.setTextFont(1);
  tft.setCursor(30, 80);
  tft.println("Good Job!!!");
  tft.setCursor(100, 180);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.printf("Nice. You stood up %d times today.\n", countStandUps);
  tft.setTextColor(TFT_WHITE);
}

void Display::drawMotivationalMsg() {
  //clear screen and set text properties
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  //draw motivational message
  tft.setTextFont(1);
  tft.setCursor(30, 50);
  tft.println("Keep going!");  //refactor these to print published msgs from broker
  tft.setCursor(30, 110);
  tft.println("You can do it!!");
  tft.setCursor(30, 170);
  tft.println("You're doing great!!!");
}

void Display::drawMotivationalMsg(String userDefinedMsg){
  //clear screen and set text properties
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  //draw motivational message
  tft.setTextFont(1);
  tft.setCursor(30, 100);
  tft.print(userDefinedMsg);
  tft.println();
}

void Display::drawDeskBuddyLogo(const char* deskBuddyLogo) {
  //draws deskBuddy logo
  tft.setCursor(0, 100);
  tft.print(deskBuddyLogo);
}

void Display::drawAuthorsMsg() {
  //signature listing developers of the deskBuddy project
  tft.setCursor(20, 190);
  tft.print("Authors:");
  tft.setCursor(70, 200);
  tft.print("Nasit Vurgun");
  tft.setCursor(70, 210);
  tft.print("Rizwan Rafique");
  tft.setCursor(70, 220);
  tft.print("Joel Celen");
  tft.setCursor(185, 200);
  tft.print("Ahmad Haj Ahmad");
  tft.setCursor(185, 210);
  tft.print("Malte Bengtsson");
  tft.setCursor(185, 220);
  tft.print("Karl Eriksson");
}