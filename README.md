## Project

<img src="https://i.imgur.com/kZWLL0f.jpg" alt="Logo" width="1000"/>  

## Description

"DeskBuddy" is an environment monitoring system to help people who sit and work. The system has two components: the Wio device that monitors the environment and notifies the user, and the Android app where users can define their optimal environment profiles and select their notifications.

The Wio device continuously monitors temperature, humidity, and light levels. These are displayed graphically at the desk of the user, and indicators change color according to user-defined preferences. The sensor data is sent to the Android app in real-time where they are displayed on the home page, and stored in a database from which users can obtain and plot historical measurements. The Wio device also periodically notifies the user that they have been sitting too long and that they need to stand up and stretch. Users can continue working and monitoring their environment after pressing a button. The device also displays one-time or periodic user-defined notifications, as well as motivational messages, which can be defined or customized in the Android app by the user.

Through environmental monitoring, motivational messaging, and user defined notifications, DeskBuddy allows users to work smarter, stay hydrated, and get regular physical movement.

## Getting Started and Installation
To be able to use this system, you need a [Wio Terminal](https://www.seeedstudio.com/Wio-Terminal-p-4509.html), the contents of this repository built and installed, and the following sensors:

1. [Grove - Temperature & Humidity Sensor (DHT11)](https://wiki.seeedstudio.com/Grove-TemperatureAndHumidity_Sensor/)
2. [Grove - Light Sensor v1.2](https://wiki.seeedstudio.com/Grove-Light_Sensor/)
3. [Grove - Multi Color Flash LED (5mm)](https://www.seeedstudio.com/Grove-Multi-Color-Flash-LED-5mm.html)

For further information on installation please see the following wiki pages:
- [Configure Wio Sensors](https://git.chalmers.se/courses/dit113/2023/group-8/desk-buddy/-/wikis/Configure%20Wio%20Sensors)
- [Configure WiFi and MQTT](https://git.chalmers.se/courses/dit113/2023/group-8/desk-buddy/-/wikis/Configure%20WiFi%20and%20MQTT)
- [Automated Build](https://git.chalmers.se/courses/dit113/2023/group-8/desk-buddy/-/wikis/Automated-Build)

## Purpose and Benefits

Since the pandemic, a lot of people have started working more or exclusively from their homes. Factors such as light, temperature, humidity affect people’s well being and it is something that is often forgotten or overlooked. Other factors that affect people’s well being are posture and taking breaks. 

The purpose of this project is to improve people's health, and their working or studying environment while they are working or studying from home. 

Through sensors to measure various factors in their home, and consecutive reminders to check their posture and take breaks. We believe this system will help a lot of people improve their working environment, and benefit them feel better while they are working out of the comfort of their own homes. 

## System Diagram

<img src="https://i.imgur.com/AXQa1BL.png" alt="Logo" width="1000"/>  

## Project status

[+ In development. +]

Expected delivery of `v1.0`: [+ May 19th, 2023 +]

## Authors and Contributions
### [Karl Eriksson](https://git.chalmers.se/kaeriks)
### Contributions
- Setting up MQTT class for the application
- Setting up the reminders screen for saving various reminder intervals
- Setting up the silent mode for reminders
- Designing logo for the app and setting up the Icon displayed in the phone, and the start screen when you open the app
- Contributing to various wiki page articles and readme
- Adding enum classes to the app for easier use of topics and colors that should be published to the Wio

### [Malte Bengtsson](https://git.chalmers.se/bmalte)
### Contributions
- Implementing physical button behavior and functionality 
- Creating home page of the app with behavior and GUI appearance connected to this
- Creating suggestionsbutton and corresponding page with information regarding sensor settings 
 

### [Ahmad Haj Ahmad](https://git.chalmers.se/haja)
### Contributions
- 

### [Nasit Vurgun](https://git.chalmers.se/nasit)
### Contributions
- Wio Terminal (majority of features):
   - Encrypted MQTT connection (SSL/TLS) to HiveMQ cloud broker
      - BrokerInfo.h file with secrets info and SSL certificate
      - MqttTopics.h file with topics
      - MQTT methods - subscribe to 7 topics, publish to 4 topics
      - Parser methods
      - Non-blocking delay implementation with MQTT - crucial for high-throughput messaging without lag (listen while waiting)
   - Wi-Fi reconnection method with Rizwan
   - Refactoring code from procedural to OOP (as much as possible)
      - Event class - standUp, motivate, notification, publish, refreshDisplay
      - Sensor class - store sensor values, indicator colors, and user preferences
      - Display class - draw methods for setup screen, dashboard, events, removing flicker for high refresh rates
      - LED class
      - Buzzer class
      - Button class with Malte
   - Implement Controller in arduino_src.ino loop() to trigger Views: standUp, motivate, notification, publish, refreshDisplay event sequences
   - Timing of events using millis() to achieve multitasking, precise timing, and responsive code execution of multiple event sequences, each running with multiple timers in parallel
   - Implementing Wio logic for user defined preferences (user-defined timing intervals, messages, environmental preferences)
   - Fix runtime errors in Wio Terminal (such as message corruption - fix through immutable String objects)
- DeskBuddy Android App:
    - Populate database with sensor data
    - Co-contribute to temperature chart (database queries) with Ahmad
    - Fix runtime errors in Android Application
    - Improve GUI of MainActivity class

### [Joel Celén](https://git.chalmers.se/joelcel)
### Contributions
-
### [Rizwan Rafiq](https://git.chalmers.se/rizwanra)
### Contributions
-

<img src="https://i.imgur.com/IoqlBfD.png" alt="Logo" width="100"/>
