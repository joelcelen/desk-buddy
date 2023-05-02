#include "Notification.h"

// Constructors
Notification::Notification() {
  notificationMessage = "This is a notification.";     //default notification msg
  intervalNotification = 3600000;                      //one hour
  lastNotification = 0;
}

Notification::Notification(const char* message, unsigned long interval) {
  notificationMessage = message;                       //user defined notification msg
  intervalNotification = interval;
  lastNotification = 0;
}

// Getters
const char* Notification::getMessage() {
  return notificationMessage;
}

unsigned long Notification::getInterval() {
  return intervalNotification;
}

unsigned long Notification::getLastNotification() {
  return lastNotification;
}

// Setters
void Notification::setMessage(const char* message) {
  notificationMessage = message;
}

void Notification::setInterval(unsigned long interval) {
  intervalNotification = interval;
}

void Notification::setLastNotification(unsigned long lastNotification) {
  this->lastNotification = lastNotification;
}