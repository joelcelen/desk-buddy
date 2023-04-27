package com.example.deskbuddyapp;


/** Class for saving user preferences. **/

public class RoomProfile {

    private String roomName;
    private double temperature;
    private double humidity;
    private double lightLevel;

    // Constructor with specific values
    public RoomProfile(String roomName, double temperature, double humidity, double lightLevel){
        this.roomName = roomName;
        this.temperature = temperature;
        this.humidity = humidity;
        this.lightLevel = lightLevel;
    }

    // Constructor with default values.
    public RoomProfile(){
        this.roomName = "Default";
        this.temperature = 23.0;
        this.humidity = 40.0;
        this.lightLevel = 700;
    }

    // Getters and setters.
    public String getRoomName() {
        return roomName;
    }

    public double getTemperature() {
        return temperature;
    }

    public double getHumidity() {
        return humidity;
    }

    public double getLightLevel() {
        return lightLevel;
    }

    public void setRoomName(String roomName) {
        this.roomName = roomName;
    }

    public void setTemperature(double temperature) {
        this.temperature = temperature;
    }

    public void setHumidity(double humidity) {
        this.humidity = humidity;
    }

    public void setLightLevel(double lightLevel) {
        this.lightLevel = lightLevel;
    }
}
