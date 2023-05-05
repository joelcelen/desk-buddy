#include "Event.h"

// Constructors
Event::Event() {
  eventMessage = "";     			      //default event msg (empty string)
  intervalEvent = 3600000;          //default interval: 1 hour
  lastEvent = 0;				            //last event time
  countEvents = 0;                  // event counter
}

Event::Event(const char* message, unsigned long interval) {
  eventMessage = message;          //user defined event msg
  intervalEvent = interval;		     //user defined event interval
  lastEvent = 0;				           //last event time
  countEvents = 0;                 // event counter
}

Event::Event(const char*message){
  Event();                        //call default constructor first
  setMessage(message);
}

Event::Event(unsigned long interval){
  Event();                        //call default constructor first
  setInterval(interval);
}

// Checks if event should execute
bool Event::shouldExecute() {
  if(millis() - lastEvent > intervalEvent){
    return true;
  }
  return false;
}

// Getters
const char* Event::getMessage() {
  return eventMessage.c_str();
}

unsigned long Event::getInterval() {
  return intervalEvent;
}

unsigned long Event::getLastEvent() {
  return lastEvent;
}

int Event::getCount(){
    return countEvents;
}

// Setters
void Event::setMessage(const char* message) {
  eventMessage = String(message);
  countEvents = 0;
}

void Event::setInterval(unsigned long interval) {
  intervalEvent = interval;
}

void Event::setLastEvent(unsigned long lastEvent) {
  this->lastEvent = lastEvent;
  countEvents++;
}