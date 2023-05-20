## Project

<img src="https://i.imgur.com/kZWLL0f.jpg" alt="Logo" width="1000"/>  

## Description

"DeskBuddy" is an environment monitoring system to help people who sit and work. The system has two components: the Wio device that monitors the environment and notifies the user, and the Android app where users can define their optimal environment profiles and select their notifications.

The Wio device continuously monitors temperature, humidity, and light levels. These are displayed graphically at the desk of the user, and indicators change color according to user-defined preferences. The sensor data is sent to the Android app in real-time where they are displayed on the home page, and stored in a database from which users can obtain and plot historical measurements. The Wio device also periodically notifies the user that they have been sitting too long and that they need to stand up and stretch. Users can continue working and monitoring their environment after pressing a button. The device also displays one-time or periodic user-defined notifications, as well as motivational messages, which can be defined or customized in the Android app by the user.

Through environmental monitoring, motivational messaging, and user defined notifications, DeskBuddy allows users to work smarter, stay hydrated, and get regular physical movement.

## Getting Started and Installation
To be able to use this system, you need a [Wio Terminal](https://www.seeedstudio.com/Wio-Terminal-p-4509.html), the contents of this repository built and installed, and the following sensors:

- [Grove - Temperature & Humidity Sensor (DHT11)](https://wiki.seeedstudio.com/Grove-TemperatureAndHumidity_Sensor/)
- [Grove - Light Sensor v1.2](https://wiki.seeedstudio.com/Grove-Light_Sensor/)
- [Grove - Multi Color Flash LED (5mm)](https://www.seeedstudio.com/Grove-Multi-Color-Flash-LED-5mm.html)

For further information on installation please see the following wiki pages:
1. [Configure Wio Sensors](https://git.chalmers.se/courses/dit113/2023/group-8/desk-buddy/-/wikis/Configure%20Wio%20Sensors)
2. [Configure WiFi and MQTT](https://git.chalmers.se/courses/dit113/2023/group-8/desk-buddy/-/wikis/Configure%20WiFi%20and%20MQTT)
3. [Automated Build Instructions](https://git.chalmers.se/courses/dit113/2023/group-8/desk-buddy/-/wikis/Automated%20Build%20Instructions)

## Purpose and Benefits

Since the pandemic, a lot of people have started working more or exclusively from their homes. Factors such as light, temperature, humidity affect people’s well being and it is something that is often forgotten or overlooked. Other factors that affect people’s well being are posture and taking breaks. 

The purpose of this project is to improve people's health, and their working or studying environment while they are working or studying from home. 

Through sensors to measure various factors in their home, and consecutive reminders to check their posture and take breaks. We believe this system will help a lot of people improve their working environment, and benefit them feel better while they are working out of the comfort of their own homes. 

## System Diagram

<img src="https://i.imgur.com/m4MkGQa.png" alt="Logo" width="1000"/>  

## Project status
[![Release](https://img.shields.io/badge/release-v1.1-blue.svg)](https://git.chalmers.se/courses/dit113/2023/group-8/desk-buddy/-/tags) -   Final Sprint Delivery with CI for Arduino / Wio Terminal: 2023-05-20

[![Release](https://img.shields.io/badge/release-v1.0-blue.svg)](https://git.chalmers.se/courses/dit113/2023/group-8/desk-buddy/-/tags) - Final Sprint Delivery (without CI): 2023-05-19

## Video Demo
### Link to [video demo](https://youtu.be/bD9ji7lhYXw)

## Authors and Contributions
### [Karl Eriksson](https://git.chalmers.se/kaeriks)
### Contributions
- Setting up MQTT class for the application
- Making sure Mqtt connection is not publicly available, gets SSL certificate, and username/password from file in gitignore when starting app
- Setting up the reminders screen for saving various reminder intervals with Rizwan
- Setting up the silent mode for reminders
- Designing logo for the app and setting up the Icon displayed in the phone, and the start screen when you open the app
- Contributing to various wiki page articles and readme
- Adding enum classes to the app for easier use of topics and colors that should be published to the Wio

### [Malte Bengtsson](https://git.chalmers.se/bmalte)
### Contributions
- Wio Terminal:
   - Implementing physical button behavior and functionality 
   - Refactor button into OOP structure with .h and .cpp which are called in the .ino in Arduino
- Android App:
   - Creating home page of the app with behavior and GUI appearance connected to this
   - Creating suggestionsbutton and corresponding page with information regarding sensor settings 
   - Overall GUI refactoring/edits
- Contributing to various wiki pages of the git 
   - Sprint pages 1, 2, 3
   - Installation
   - Code review guidelines
   - Description
   - Requirements
- Contributing to README page
- Working on the CI/CD and pipeline of the project's git. 
   - The pipeline implementation unfortunately did not work as intended, even after extensive testing and             troubleshooting.
 

### [Ahmad Haj Ahmad](https://git.chalmers.se/haja)
### Contributions
- Setting up MQTT class for the application with Karl.
- Setting up the configurion process of populating the database with sensor measurements.
- Setting up the the graph activites of the three indoor measurements.
- Improve the GUI (All activities) after the second sprint (The non-final refinement).
- Contributing to various wiki page articles and readme (The commit and code review guidlines).
- Setting up the light sensor code (The first version)


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
   - Timing of events using millis() to achieve multitasking, precise timing, and responsive code execution of multiple event sequences, each running with own timers in parallel
   - Implementing Wio logic for user defined preferences (user-defined timing intervals, messages, environmental preferences)
   - Fix runtime errors in Wio Terminal (such as message corruption - fix through immutable String objects)
- DeskBuddy Android App:
    - Populate database with sensor data
    - Co-contribute to temperature chart (database queries) with Ahmad
    - Fix runtime errors in Android Application
    - Improve GUI of MainActivity class
- Contributions to sprint demos, README and Wiki (installation pages: configuring sensors, WiFi/MQTT setup, automated build instructions)
- Continuous integration (CI) for Arduino / Wio Terminal
- Continuous integration (CI) for Android App with Malte, Joel, Kalle

### [Joel Celén](https://git.chalmers.se/joelcel)
### Contributions
-
### [Rizwan Rafiq](https://git.chalmers.se/rizwanra)
### Contributions
-

<img src="https://i.imgur.com/IoqlBfD.png" alt="Logo" width="100"/>
