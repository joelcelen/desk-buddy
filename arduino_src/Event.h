/**************************************************************
* "Event.h" - a library for defining Event objects that can be used to represent a state that
*  transitions at event specific time intervals. Each Event state corresponds to a View in the  
*  MVC pattern, or a state in finite state machine. Each state has an event message, and a counter
*  for how many times the state was entered. States keep track of when they were last exited, 
*  and there is a shouldExecute() method checking if the state can be entered again.
 **************************************************************/
#ifndef EVENT_H
#define EVENT_H

#include <Arduino.h> // need this library to include millis() for timing

class Event {
  private:
    String eventMessage;         //event message
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