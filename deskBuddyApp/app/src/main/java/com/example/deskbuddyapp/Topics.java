package com.example.deskbuddyapp;

//enums for the colors that will be published to the Wio terminal from the app
public enum Topics {
    PUB_SUB("deskBuddy"), //publish-subscribe to deskBuddy topic
    TEMP_SUB("deskBuddy/temperature"), //subscribe to temperature sensor readings from Wio terminal
    HUMIDITY_SUB("deskBuddy/humidity"), //subscribe to humidity sensor readings from Wio terminal
    LIGHT_SUB("deskBuddy/light"), //subscribe to light sensor readings from Wio terminal
    LIGHT_PUB("deskBuddy/app/setLight"),  //publish light preferences to Wio terminal
    TEMP_PUB("deskBuddy/app/setTemperature"), //publish temperature preferences to Wio terminal
    HUMIDITY_PUB("deskBuddy/app/setHumidity"), //publish humidity preferences to Wio terminal
    NOTIFICATION_PUB("deskBuddy/app/setNotification"), //publish notification preferences to Wio terminal
    MOTIVATION_PUB("deskBuddy/app/setMotivation"); //publish motivation message preferences to Wio terminal
    //MOOD_PUB("deskBuddy/app/setMood"); //publish mood preferences to Wio terminal might be needed in future but not for now
 //   TIMING_PUB("deskBuddy/app/setTiming"); //publish timing to Wio terminal, might be needed in future but not for now
    private final String topic;

    Topics(String topic) {
        this.topic = topic;
    }
    public String getTopic() {
        return topic;
    }
}