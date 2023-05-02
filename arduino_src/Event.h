#ifndef EVENT_H
#define EVENT_H

#include <Arduino.h> // need this library to include millis() for timing

class Event {
  private:
    const char* eventMessage;         //event message
    unsigned long intervalEvent;      //time interval to send event to user (default: 3600000)
    unsigned long lastEvent;          //last event send time
    int countEvents;                  //event counter

  public:
    // Constructors
    Event();
    Event(const char* message);
    Event(unsigned long interval);
    Event(const char* message, unsigned long interval);

    // Checks if method should execute
    bool shouldExecute();

    // Getters
    const char* getMessage();
    unsigned long getInterval();
    unsigned long getLastEvent();
    int getCount();

    // Setters
    void setMessage(const char* message);
    void setInterval(unsigned long interval);
    void setLastEvent(unsigned long lastEvent);
};

#endif // NOTIFICATION_H